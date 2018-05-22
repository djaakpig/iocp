#include "IoCallbackAccept.h"
#include "Socket.h"
#include "TcpListener.h"
#include "TcpSession.h"
#include <MSWSock.h>

void IoCallbackAccept::Bind(shared_ptr<TcpSession> sessionPtr, const IoCallbackFn&& fn, shared_ptr<TcpListener> listenerPtr)
{
	_listenerPtr = listenerPtr;
	_sessionPtr = sessionPtr;
	_fn = fn;
}

void IoCallbackAccept::Clear()
{
	__super::Clear();
	_listenerPtr = nullptr;
}

const Socket* IoCallbackAccept::GetListenerSocket() const
{
	return _listenerPtr ? _listenerPtr->GetSocket() : nullptr;
}

bool IoCallbackAccept::OnComplete(const int e, const DWORD)
{
	if(e) return _fn(e, _sessionPtr);

	if(!_listenerPtr->ImbueContextTo(_sessionPtr->GetSocket()))
	{
		_fn(WSAGetLastError(), _sessionPtr);
		return false;
	}

	PSOCKADDR pLocalSockaddr = nullptr, pRemoteSockaddr = nullptr;
	int localSockaddrLen = 0, remoteSockaddrLen = 0;

	GetAcceptExSockaddrs(_buf, 0,
						 LocalSockaddrLen, RemoteSockaddrLen,
						 &pLocalSockaddr, &localSockaddrLen,
						 &pRemoteSockaddr, &remoteSockaddrLen);

	_sessionPtr->SetLocalSockaddr(*pLocalSockaddr);
	_sessionPtr->SetRemoteSockaddr(*pRemoteSockaddr);

	return _fn(ERROR_SUCCESS, _sessionPtr);
}

bool IoCallbackAccept::Post()
{
	const auto r = AcceptEx(_listenerPtr->GetSocket()->GetSocketHandle(),
							_sessionPtr->GetSocket()->GetSocketHandle(),
							_buf,
							0,
							LocalSockaddrLen,
							RemoteSockaddrLen,
							nullptr,
							this);

	if(SOCKET_ERROR == r)
	{
		const auto lastError = WSAGetLastError();
		if(WSA_IO_PENDING != lastError)
			return false;
	}

	return true;
}