#include "IoCallbackSend.h"
#include "Socket.h"
#include "TcpSession.h"

void IoCallbackSend::Bind(shared_ptr<TcpSession> sessionPtr, const IoCallbackFnSend&& fn, const WSABUF& buf)
{
	_sessionPtr = sessionPtr;
	_fn = fn;
	_buf = buf;
}

bool IoCallbackSend::OnComplete(const int e, const DWORD)
{
	if(e) return _fn(e, _sessionPtr, _buf);

	const auto s = _sessionPtr->GetSocket()->GetSocketHandle();
	const auto r = _Send(s, _buf.buf + _numSentBytes, _buf.len - _numSentBytes);

	if(ERROR_SUCCESS != r.first || 0 == r.second)
		return _fn(r.first, _sessionPtr, _buf);

	_numSentBytes += r.second;

	if(_numSentBytes < _buf.len)
		return Post();

	return _fn(r.first, _sessionPtr, _buf);
}

bool IoCallbackSend::Post()
{
	const DWORD flags = 0;
	WSABUF wsaBuf{0, nullptr};
	const auto r = WSASend(_sessionPtr->GetSocket()->GetSocketHandle(), &wsaBuf, 1, nullptr, flags, this, nullptr);

	if(SOCKET_ERROR == r)
	{
		const auto lastError = WSAGetLastError();
		if(WSA_IO_PENDING != lastError)
			return false;
	}

	return true;
}

pair<int, DWORD> IoCallbackSend::_Send(const SOCKET s, char* const pBuf, const int sz) const
{
	auto pCurrentBuf = pBuf;
	auto remainSize = sz;

	while(0 < remainSize)
	{
		const auto r = ::send(s, pCurrentBuf, remainSize, 0);

		if(0 == r)
			return{WSAECONNRESET, 0};

		if(SOCKET_ERROR == r)
		{
			const auto lastError = WSAGetLastError();
			if(WSAEWOULDBLOCK == lastError)
				break;

			return{lastError, 0};
		}

		pCurrentBuf += r;
		remainSize -= r;
	}

	return{ERROR_SUCCESS, sz - remainSize};
}