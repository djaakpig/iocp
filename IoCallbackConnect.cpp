#include "IoCallbackConnect.h"
#include <WS2tcpip.h>
#include "ExtensionTable.h"
#include "Socket.h"
#include "TcpSession.h"

bool IoCallbackConnect::OnComplete( const int e )
{
	const auto r = _Invoke( e, _sessionPtr );
	ResetInProgress();
	return r;
}

bool IoCallbackConnect::Post( const ExtensionTable& extensionTable )
{
	const auto r = extensionTable.connectEx( _sessionPtr->GetSocket()->GetSocketHandle(),
											 _addr.ToSockAddrPtr(),
											 _addr.GetSize(),
											 nullptr,
											 0,
											 nullptr,
											 this );

	if( !r )
	{
		const auto lastError = WSAGetLastError();
		if( WSA_IO_PENDING != lastError )
		{
			Clear();
			return false;
		}
	}

	SetInProgress();

	return true;
}