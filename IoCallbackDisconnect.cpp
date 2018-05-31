#include "IoCallbackDisconnect.h"
#include "Socket.h"
#include "ExtensionTable.h"
#include "TcpSession.h"

void IoCallbackDisconnect::OnComplete( const int e )
{
	_Invoke( e, _sessionPtr );
	Clear();
}

bool IoCallbackDisconnect::Post( const shared_ptr<ExtensionTable>& extensionTablePtr )
{
	const auto r = extensionTablePtr->disconnectEx( _sessionPtr->GetSocket()->GetSocketHandle(), this, TF_REUSE_SOCKET, 0 );

	if( !r )
	{
		const auto lastError = WSAGetLastError();
		if( WSA_IO_PENDING != lastError )
		{
			const auto thisPtr = shared_from_this();
			if( !_sessionPtr->PostError( lastError, thisPtr ) )
				return false;
		}
	}

	return true;
}