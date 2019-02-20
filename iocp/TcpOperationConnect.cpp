#include "TcpOperationConnect.h"
#include <WS2tcpip.h>
#include "WinsockExtension.h"
#include "Socket.h"
#include "TcpSession.h"

void TcpOperationConnect::OnComplete( const int e )
{
	const auto r = _Invoke( e, _sessionPtr );

	if( !r )
		_sessionPtr->Disconnect();

	Clear();
}

bool TcpOperationConnect::Post( const std::shared_ptr<WinsockExtension>& exPtr )
{
	const auto r = exPtr->connectEx( _sessionPtr->GetSocket()->GetValue(),
									 _addr.ToSockAddrPtr(),
									 _addr.GetSize(),
									 nullptr,
									 0,
									 nullptr,
									 this );

	return r ? true : _HandleError();
}