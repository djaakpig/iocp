#include "TcpOperationRecv.h"
#include "Socket.h"
#include "TcpSession.h"

TcpOperationRecv::TcpOperationRecv(const uint32_t capacity) :
	_buf{capacity}
{
}

bool TcpOperationRecv::BeginRead(WSABUF& wsaBuf)
{
	return _buf.BeginRead(wsaBuf);
}

void TcpOperationRecv::Clear()
{
	_buf.Clear();
	__super::Clear();
}

void TcpOperationRecv::EndRead(const uint32_t numReadBytes)
{
	_buf.EndRead(numReadBytes);
}

void TcpOperationRecv::OnComplete(const int32_t e)
{
	const auto r = _OnComplete(e);

	if(!r || !Post())
	{
		_session->Disconnect();
		Clear();
	}
}

bool TcpOperationRecv::Post()
{
	DWORD flags = 0;
	WSABUF wsaBuf{0, nullptr};
	const auto r = WSARecv(_session->GetSocket()->GetValue(), &wsaBuf, 1, nullptr, &flags, this, nullptr);

	return SOCKET_ERROR != r ? true : _HandleError();
}

bool TcpOperationRecv::_OnComplete(const int32_t e)
{
	if(e)
		return _Invoke(e, _session);

	WSABUF wsaBuf{};
	std::pair<int32_t, uint32_t> r{ERROR_SUCCESS, 0};

	while(ERROR_SUCCESS == r.first && _buf.BeginWrite(wsaBuf))
	{
		r = _Read(wsaBuf);

		if(0 == r.second)
			break;

		_buf.EndWrite(r.second);
	}

	return _Invoke(r.first, _session);
}

auto TcpOperationRecv::_Read(WSABUF& wsaBuf) const->std::pair<int32_t, uint32_t>
{
	const auto s = _session->GetSocket()->GetValue();
	auto pCurrentBuf = wsaBuf.buf;
	auto remainSize = wsaBuf.len;

	while(0 < remainSize)
	{
		const auto r = ::recv(s, pCurrentBuf, remainSize, 0);

		if(0 == r)
			return{WSAECONNRESET, wsaBuf.len - remainSize};

		if(SOCKET_ERROR == r)
		{
			const auto lastError = WSAGetLastError();
			if(WSAEWOULDBLOCK == lastError)
				break;

			return{lastError, wsaBuf.len - remainSize};
		}

		pCurrentBuf += r;
		remainSize -= r;
	}

	return{ERROR_SUCCESS, wsaBuf.len - remainSize};
}