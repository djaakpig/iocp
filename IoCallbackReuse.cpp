#include "IoCallbackReuse.h"
#include "Socket.h"
#include "TcpSession.h"
#include <MSWSock.h>

void IoCallbackReuse::Bind(shared_ptr<TcpSession> sessionPtr, const IoCallbackFn&& fn)
{
	_sessionPtr = sessionPtr;
	_fn = fn;
}

bool IoCallbackReuse::OnComplete(const int e, const DWORD)
{
	return _fn(e, _sessionPtr);
}

bool IoCallbackReuse::Post()
{
	const DWORD flags = TF_DISCONNECT | TF_REUSE_SOCKET;
	const auto r = TransmitFile(_sessionPtr->GetSocket()->GetSocketHandle(), nullptr, 0, 0, this, nullptr, flags);

	if(!r)
	{
		const auto lastError = WSAGetLastError();
		if(WSA_IO_PENDING != lastError)
			return false;
	}

	return true;
}