#include "IoCallbackDisconnect.h"
#include "Socket.h"
#include "ExtensionTable.h"
#include "TcpSession.h"

void IoCallbackDisconnect::OnComplete( const int e )
{
	const auto adjustedError = ERROR_NETNAME_DELETED == e ? ERROR_SUCCESS : e;

	_Invoke( adjustedError, _sessionPtr );
	Clear();
}

bool IoCallbackDisconnect::Post( const shared_ptr<ExtensionTable>& extensionTablePtr )
{
	const auto r = extensionTablePtr->disconnectEx( _sessionPtr->GetSocket()->GetValue(), this, TF_REUSE_SOCKET, 0 );

	return r ? true : _HandleError();
}