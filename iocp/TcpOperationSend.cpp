#include "TcpOperationSend.h"
#include "Socket.h"
#include "TcpSession.h"
#include "WsaBuf.h"

void TcpOperationSend::Enqueue(const std::shared_ptr<WsaBuf>& buf)
{
	const std::unique_lock<std::mutex> l(_lock);
	_bufs.emplace_back(buf);
}

void TcpOperationSend::OnComplete(const int32_t e)
{
	const auto r = _OnComplete(e);

	if(!r)
	{
		_session->Disconnect();
		Clear();
	}
}

bool TcpOperationSend::Post()
{
	_numSentBytes = 0;

	const uint32_t flags = 0;
	WSABUF wsaBuf{0, nullptr};
	const auto r = WSASend(_session->GetSocket()->GetValue(), &wsaBuf, 1, nullptr, flags, this, nullptr);

	return SOCKET_ERROR != r ? true : _HandleError();
}

bool TcpOperationSend::_OnComplete(const int32_t e)
{
	if(e)
	{
		DoExclusive(_lock, [this, e]
		{
			_Invoke(e, _session);

			_bufs.clear();
			_numSentBytes = 0;
		});

		return false;
	}

	auto numSentBytes = _numSentBytes;
	BufferPtrList bufs;
	DoExclusive(_lock, [&]
	{
		_bufs.swap(bufs);
	});

	while(!bufs.empty())
	{
		const auto& buf = bufs.front()->Get();
		const auto r = _Send(buf.buf + numSentBytes, buf.len - numSentBytes);

		numSentBytes += r.second;

		if(ERROR_SUCCESS != r.first)
		{
			_Invoke(r.first, _session);

			break;
		}

		if(0 == r.second)
			break;

		if(numSentBytes < buf.len)
			break;

		_Invoke(ERROR_SUCCESS, _session);

		bufs.pop_front();
		numSentBytes = 0;
	}

	_numSentBytes = numSentBytes;

	return DoExclusive(_lock, [this, &bufs]
	{
		_bufs.insert(_bufs.begin(), bufs.rbegin(), bufs.rend());

		if(_bufs.empty())
		{
			Clear();
			return true;
		}

		return Post();
	});
}

auto TcpOperationSend::_Send(char* const pBuf, const int32_t sz) const->std::pair<int32_t, uint32_t>
{
	const auto s = _session->GetSocket()->GetValue();
	auto pCurrentBuf = pBuf;
	auto remainSize = sz;

	while(0 < remainSize)
	{
		const auto r = ::send(s, pCurrentBuf, remainSize, 0);

		if(0 == r)
			return{WSAECONNRESET, sz - remainSize};

		if(SOCKET_ERROR == r)
		{
			const auto lastError = WSAGetLastError();
			if(WSAEWOULDBLOCK != lastError)
				return{lastError, sz - remainSize};

			break;
		}

		pCurrentBuf += r;
		remainSize -= r;
	}

	return{ERROR_SUCCESS, sz - remainSize};
}