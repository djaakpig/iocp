#include "IoCallbackConnect.h"
#include <WS2tcpip.h>
#include "ExtensionTable.h"
#include "Socket.h"
#include "TcpSession.h"

void IoCallbackConnect::OnComplete( const int e )
{
	const auto r = _Invoke( e, _sessionPtr );

	if( !r )
		_sessionPtr->Disconnect();

	Clear();
}

bool IoCallbackConnect::Post( const shared_ptr<ExtensionTable>& extensionTablePtr )
{
	const auto r = extensionTablePtr->connectEx( _sessionPtr->GetSocket()->GetSocketHandle(),
												 _addr.ToSockAddrPtr(),
												 _addr.GetSize(),
												 nullptr,
												 0,
												 nullptr,
												 this );

	if( r )
		return true;

	return _HandleError( WSAGetLastError() );
}