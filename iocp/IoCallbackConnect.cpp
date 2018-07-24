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
	const auto r = extensionTablePtr->connectEx( _sessionPtr->GetSocket()->GetValue(),
												 _addr.ToSockAddrPtr(),
												 _addr.GetSize(),
												 nullptr,
												 0,
												 nullptr,
												 this );

	return r ? true : _HandleError();
}