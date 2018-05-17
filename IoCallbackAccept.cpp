#include "IoCallbackAccept.h"
#include "TcpListener.h"
#include "TcpSession.h"
#include <MSWSock.h>

void IoCallbackAccept::Clear()
{
	__super::Clear();
	_listenerPtr = nullptr;
}

const Socket* IoCallbackAccept::GetListenerSocket() const
{
	return _listenerPtr ? _listenerPtr->GetSocket() : nullptr;
}

bool IoCallbackAccept::OnComplete(const int e, const DWORD numBytes)
{
	if(e) return _Invoke(e, numBytes);

	if(!_listenerPtr->ImbueContextTo(_sessionPtr->GetSocket()))
	{
		_Invoke(WSAGetLastError(), numBytes);
		return false;
	}

	PSOCKADDR pLocalSockaddr = nullptr, pRemoteSockaddr = nullptr;
	int localSockaddrLen = 0, remoteSockaddrLen = 0;
	GetAcceptExSockaddrs(_buf, 0,
						 LocalSockaddrLen, RemoteSockaddrLen,
						 &pLocalSockaddr, &localSockaddrLen,
						 &pRemoteSockaddr, &remoteSockaddrLen);

	return _Invoke(ERROR_SUCCESS, numBytes);
}