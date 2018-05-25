#include "TcpSession.h"
#include "Global.h"
#include "Socket.h"
#include "TcpListener.h"
#include "IoCallbackAccept.h"
#include "IoCallbackConnect.h"
#include "IoCallbackDisconnect.h"
#include "IoCallbackRecv.h"
#include "IoCallbackSend.h"

TcpSession::TcpSession( const ExtensionTable& extensionTable ) : _extensionTable( extensionTable )
{
	_pSocket = new Socket();
	_pAcceptCallback = new IoCallbackAccept();
	_pConnectCallback = new IoCallbackConnect();
	_pDisconnectCallback = new IoCallbackDisconnect();
	_pRecvCallback = new IoCallbackRecv( 1024 );
	_pSendCallback = new IoCallbackSend();
}

TcpSession::~TcpSession()
{
	SafeDelete( _pAcceptCallback );
	SafeDelete( _pConnectCallback );
	SafeDelete( _pDisconnectCallback );
	SafeDelete( _pRecvCallback );
	SafeDelete( _pSendCallback );
	SafeDelete( _pSocket );
}

HANDLE TcpSession::GetHandle() const
{
	return _pSocket->GetHandle();
}

void TcpSession::SetOnAccept( const IoCallbackFn&& fn )
{
	_pAcceptCallback->SetFn( move( fn ) );
}

void TcpSession::SetOnConnect( const IoCallbackFn&& fn )
{
	_pConnectCallback->SetFn( move( fn ) );
}

void TcpSession::SetOnDisconnect( const IoCallbackFn&& fn )
{
	_pDisconnectCallback->SetFn( move( fn ) );
}

void TcpSession::SetOnRecv( const IoCallbackFnRecv&& fn )
{
	_pRecvCallback->SetFn( move( fn ) );
}

void TcpSession::SetOnSend( const IoCallbackFnSend&& fn )
{
	_pSendCallback->SetFn( move( fn ) );
}

bool TcpSession::Accept( const shared_ptr<TcpListener>& listenerPtr )
{
	if( !_pSocket->IsValid() )
		return false;

	if( _pAcceptCallback->IsInProgress() )
		return true;

	_pAcceptCallback->SetSession( shared_from_this() );
	_pAcceptCallback->SetListener( listenerPtr );

	SecureZeroMemory( &_localSockaddr, sizeof( _localSockaddr ) );
	SecureZeroMemory( &_remoteSockaddr, sizeof( _remoteSockaddr ) );

	return _pAcceptCallback->Post( _extensionTable );
}

void TcpSession::Close()
{
	_pSocket->Close();
}

bool TcpSession::Connect( const SockaddrIn& remoteAddr )
{
	if( !_pSocket->IsValid() )
		return false;

	if( _pConnectCallback->IsInProgress() )
		return true;

	_pConnectCallback->SetSession( shared_from_this() );
	_pConnectCallback->SetAddr( remoteAddr );

	return _pConnectCallback->Post( _extensionTable );
}

bool TcpSession::Create()
{
	if( !_pSocket->Create( SOCK_STREAM, IPPROTO_TCP ) )
		return false;

	return true;
}

bool TcpSession::Disconnect()
{
	if( _pDisconnectCallback->IsInProgress() )
		return true;

	_pDisconnectCallback->SetSession( shared_from_this() );

	return _pDisconnectCallback->Post( _extensionTable );
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

bool TcpSession::Recv()
{
	if( !_pSocket->IsValid() )
		return false;

	if( _pRecvCallback->IsInProgress() )
		return true;

	_pRecvCallback->SetSession( shared_from_this() );

	return _pRecvCallback->Post();
}

bool TcpSession::Send( const WSABUF& buf )
{
	if( !_pSocket->IsValid() )
		return false;

	_pSendCallback->Enqueue( buf );

	if( _pSendCallback->SetInProgress() )
		return true;

	_pSendCallback->SetSession( shared_from_this() );

	return _pSendCallback->Post();
}