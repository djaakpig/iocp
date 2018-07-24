#include "TcpOperationDisconnect.h"
#include "Socket.h"
#include "WinsockExtension.h"
#include "TcpSession.h"

void TcpOperationDisconnect::OnComplete( const int e )
{
	const auto adjustedError = ERROR_NETNAME_DELETED == e ? ERROR_SUCCESS : e;

	_Invoke( adjustedError, _sessionPtr );
	Clear();
}

bool TcpOperationDisconnect::Post( const shared_ptr<WinsockExtension>& exPtr )
{
	const auto r = exPtr->disconnectEx( _sessionPtr->GetSocket()->GetValue(), this, TF_REUSE_SOCKET, 0 );

	return r ? true : _HandleError();
}