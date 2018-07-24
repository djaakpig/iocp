#include "TcpOperationAccept.h"
#include "WinsockExtension.h"
#include "Socket.h"
#include "TcpListener.h"
#include "TcpSession.h"

void TcpOperationAccept::Clear()
{
	_listenerPtr = nullptr;

	__super::Clear();
}

void TcpOperationAccept::FillAddrTo( const shared_ptr<WinsockExtension>& exPtr,
									 PSOCKADDR* const ppRemoteSockaddr,
									 PSOCKADDR* const ppLocalSockaddr )
{
	int localSockaddrLen = 0;
	int remoteSockaddrLen = 0;

	exPtr->getAcceptExSockaddrs( _buf, 0, SockaddrLen, SockaddrLen, ppLocalSockaddr, &localSockaddrLen, ppRemoteSockaddr, &remoteSockaddrLen );
}

void TcpOperationAccept::OnComplete( const int e )
{
	const auto r = _OnComplete( e );

	if( !r )
		_sessionPtr->Disconnect();

	Clear();
}

bool TcpOperationAccept::Post( const shared_ptr<WinsockExtension>& exPtr )
{
	DWORD bytesReceived = 0;
	const auto r = exPtr->acceptEx( _listenerPtr->GetSocket()->GetValue(),
									_sessionPtr->GetSocket()->GetValue(),
									_buf,
									0,
									SockaddrLen,
									SockaddrLen,
									&bytesReceived,
									this );

	return r ? true : _HandleError();
}

bool TcpOperationAccept::_OnComplete( const int e )
{
	if( e )
		return _Invoke( e, _sessionPtr );

	if( !_listenerPtr->SetContextTo( _sessionPtr->GetSocket() ) )
		return _Invoke( WSAGetLastError(), _sessionPtr );

	_sessionPtr->FillAddr();

	return _Invoke( ERROR_SUCCESS, _sessionPtr );
}