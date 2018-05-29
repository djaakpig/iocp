#include "IoCallbackAccept.h"
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

bool IoCallbackAccept::Post( const ExtensionTable& extensionTable )
{
	DWORD bytesReceived = 0;
	const auto r = extensionTable.acceptEx( _listenerPtr->GetSocket()->GetSocketHandle(),
											_sessionPtr->GetSocket()->GetSocketHandle(),
											_buf,
											0,
											SockaddrLen,
											SockaddrLen,
											&bytesReceived,
											this );

	if( !r )
	{
		const auto lastError = WSAGetLastError();
		if( WSA_IO_PENDING != lastError )
		{
			if( !_sessionPtr->PostError( lastError, shared_from_this() ) )
				return false;
		}
	}

	return true;
}

bool IoCallbackAccept::_OnComplete( const int e )
{
	if( e )
		return _Invoke( e, _sessionPtr );

	if( !_listenerPtr->ImbueContextTo( _sessionPtr->GetSocket() ) )
	{
		_Invoke( WSAGetLastError(), _sessionPtr );
		return false;
	}

	if( !_sessionPtr->FillAddr() )
		return false;

	return _Invoke( ERROR_SUCCESS, _sessionPtr );
}