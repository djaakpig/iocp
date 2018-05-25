#include "IoCallbackDisconnect.h"
#include "Socket.h"
#include "ExtensionTable.h"
#include "TcpSession.h"

bool IoCallbackDisconnect::OnComplete( const int e )
{
	const auto r = _Invoke( e, _sessionPtr );
	ResetInProgress();
	return r;
}

bool IoCallbackDisconnect::Post( const ExtensionTable& extensionTable )
{
	const auto r = extensionTable.disconnectEx( _sessionPtr->GetSocket()->GetSocketHandle(), this, TF_REUSE_SOCKET, 0 );

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