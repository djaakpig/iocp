#include "TcpSession.h"
#include "Global.h"
#include "Socket.h"
#include "IoService.h"
#include "IoCallbackAccept.h"
#include "IoCallbackConnect.h"
#include "IoCallbackDisconnect.h"
#include "IoCallbackError.h"
#include "IoCallbackRecv.h"
#include "IoCallbackSend.h"
#include "TcpListener.h"
#include "TcpSessionService.h"

TcpSession::TcpSession( const ExtensionTable& extensionTable ) :
	_extensionTable( extensionTable )
{
	_pSocket = new Socket();
	_acceptCallback = make_shared<IoCallbackAccept>();
	_connectCallback = make_shared<IoCallbackConnect>();
	_disconnectCallback = make_shared<IoCallbackDisconnect>();
	_recvCallback = make_shared<IoCallbackRecv>( 1024 );
	_sendCallback = make_shared<IoCallbackSend>();
}

TcpSession::~TcpSession()
{
}

void TcpSession::SetOnAccept( const IoCallbackFn&& fn )
{
	_acceptCallback->SetFn( move( fn ) );
}

void TcpSession::SetOnConnect( const IoCallbackFn&& fn )
{
	_connectCallback->SetFn( move( fn ) );
}

void TcpSession::SetOnDisconnect( const IoCallbackFn&& fn )
{
	_disconnectCallback->SetFn( move( fn ) );
}

void TcpSession::SetOnRecv( const IoCallbackFnRecv&& fn )
{
	_recvCallback->SetFn( move( fn ) );
}

void TcpSession::SetOnSend( const IoCallbackFnSend&& fn )
{
	_sendCallback->SetFn( move( fn ) );
}

bool TcpSession::Accept( const shared_ptr<TcpListener>& listenerPtr )
{
	if( !_pSocket->IsValid() )
		return false;

	if( _acceptCallback->SetInProgress() )
		return true;

	_acceptCallback->SetSession( shared_from_this() );
	_acceptCallback->SetListener( listenerPtr );

	SecureZeroMemory( &_localSockaddr, sizeof( _localSockaddr ) );
	SecureZeroMemory( &_remoteSockaddr, sizeof( _remoteSockaddr ) );

	return _acceptCallback->Post( _extensionTable );
}

void TcpSession::Close()
{
	_pSocket->Close();
	_servicePtr = nullptr;
}

bool TcpSession::Connect( const SockaddrIn& remoteAddr )
{
	if( !_pSocket->IsValid() )
		return false;

	if( _connectCallback->SetInProgress() )
		return true;

	_connectCallback->SetSession( shared_from_this() );
	_connectCallback->SetAddr( remoteAddr );

	return _connectCallback->Post( _extensionTable );
}

bool TcpSession::Create( const shared_ptr<TcpSessionService>& servicePtr )
{
	if( !_pSocket->Create( SOCK_STREAM, IPPROTO_TCP ) )
		return false;

	_servicePtr = servicePtr;

	return true;
}

bool TcpSession::Disconnect()
{
	if( !_pSocket->IsValid() )
		return false;

	if( _disconnectCallback->SetInProgress() )
		return true;

	_disconnectCallback->SetSession( shared_from_this() );

	return _disconnectCallback->Post( _extensionTable );
}

bool TcpSession::FillAddr()
{
	int remoteSockaddrLen = _remoteSockaddr.GetSize();
	if( SOCKET_ERROR == getpeername( _pSocket->GetSocketHandle(), _remoteSockaddr.ToSockAddrPtr(), &remoteSockaddrLen ) )
		return false;

	int localSockaddrLen = _localSockaddr.GetSize();
	if( SOCKET_ERROR == getsockname( _pSocket->GetSocketHandle(), _localSockaddr.ToSockAddrPtr(), &localSockaddrLen ) )
		return false;

	return true;
}

bool TcpSession::PostError( const int e, const shared_ptr<IoCallbackShared>& callbackPtr )
{
	if( !_servicePtr )
	{
		callbackPtr->Clear();
		return false;
	}

	const auto pErrorCallback = new IoCallbackError();
	pErrorCallback->SetError( e );
	pErrorCallback->SetCallback( callbackPtr );

	if( !_servicePtr->GetIoService().Post( pErrorCallback ) )
	{
		pErrorCallback->Clear();
		return false;
	}

	return true;
}

bool TcpSession::Recv()
{
	if( !_pSocket->IsValid() )
		return false;

	if( _recvCallback->SetInProgress() )
		return true;

	_recvCallback->SetSession( shared_from_this() );

	return _recvCallback->Post();
}

bool TcpSession::Send( const WSABUF& buf )
{
	if( !_pSocket->IsValid() )
		return false;

	_sendCallback->Enqueue( buf );

	if( _sendCallback->SetInProgress() )
		return true;

	_sendCallback->SetSession( shared_from_this() );

	return _sendCallback->Post();
}