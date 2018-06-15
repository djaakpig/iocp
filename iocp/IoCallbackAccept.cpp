#include "IoCallbackAccept.h"
#include "ExtensionTable.h"
#include "Socket.h"
#include "TcpListener.h"
#include "TcpSession.h"

void IoCallbackAccept::Clear()
{
	_listenerPtr = nullptr;

	__super::Clear();
}

void IoCallbackAccept::OnComplete( const int e )
{
	const auto r = _OnComplete( e );

	if( !r )
		_sessionPtr->Disconnect();

	Clear();
}

bool IoCallbackAccept::Post( const shared_ptr<ExtensionTable>& extensionTablePtr )
{
	DWORD bytesReceived = 0;
	const auto r = extensionTablePtr->acceptEx( *_listenerPtr->GetSocket(), *_sessionPtr->GetSocket(), _buf,
												0,
												SockaddrLen,
												SockaddrLen,
												&bytesReceived,
												this );

	if( r )
		return true;

	return _HandleError();
}

bool IoCallbackAccept::_OnComplete( const int e )
{
	if( e )
		return _Invoke( e, _sessionPtr );

	if( !_listenerPtr->SetContextTo( _sessionPtr->GetSocket() ) )
		return _Invoke( WSAGetLastError(), _sessionPtr );

	_sessionPtr->FillAddr();

	return _Invoke( ERROR_SUCCESS, _sessionPtr );
}