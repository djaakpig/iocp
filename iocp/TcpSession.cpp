#include "TcpSession.h"
#include "ExtensionTable.h"
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

TcpSession::TcpSession()
{
	_pSocket = new Socket();
	_acceptCallback = make_shared<IoCallbackAccept>();
	_connectCallback = make_shared<IoCallbackConnect>();
	_disconnectCallback = make_shared<IoCallbackDisconnect>();
	_recvCallback = make_shared<IoCallbackRecv>( 15 );
	_sendCallback = make_shared<IoCallbackSend>();
}

TcpSession::~TcpSession()
{
	SafeDelete( _pSocket );
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

void TcpSession::SetOnSend( const IoCallbackFn&& fn )
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

	_localSockaddr.Clear();
	_remoteSockaddr.Clear();

	if( !_acceptCallback->Post( _servicePtr->GetExtension() ) )
	{
		_acceptCallback->Clear();
		return false;
	}

	return true;
}

void TcpSession::Close()
{
	_pSocket->Close();
}

bool TcpSession::Connect( const SockaddrIn& remoteAddr )
{
	if( !_pSocket->IsValid() )
		return false;

	if( _connectCallback->SetInProgress() )
		return true;

	_connectCallback->SetSession( shared_from_this() );
	_connectCallback->SetAddr( remoteAddr );

	if( !_connectCallback->Post( _servicePtr->GetExtension() ) )
	{
		_connectCallback->Clear();
		return false;
	}

	return true;
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

	if( !_disconnectCallback->Post( _servicePtr->GetExtension() ) )
	{
		_disconnectCallback->Clear();
		return false;
	}

	return true;
}

void TcpSession::FillAddr()
{
	PSOCKADDR pLocalSockaddr = nullptr;
	int pLocalSockaddrLen = 0;
	PSOCKADDR pRemoteSockaddr = nullptr;
	int pRemoteSockaddrLen = 0;

	_servicePtr->GetExtension()->getAcceptExSockaddrs( _acceptCallback->GetBuf(),
											  0,
											  SockaddrLen,
											  SockaddrLen,
											  &pLocalSockaddr,
											  &pLocalSockaddrLen,
											  &pRemoteSockaddr,
											  &pRemoteSockaddrLen );

	_localSockaddr = *pLocalSockaddr;
	_remoteSockaddr = *pRemoteSockaddr;
}

bool TcpSession::PostError( const int lastError, const shared_ptr<IoCallbackShared>& callbackPtr )
{
	if( !_servicePtr )
		return false;

	const auto pErrorCallback = new IoCallbackError();
	pErrorCallback->SetError( lastError );
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

	if( !_recvCallback->Post() )
	{
		_recvCallback->Clear();
		return false;
	}

	return true;
}

bool TcpSession::Send( const shared_ptr<WsaBuf>& buf )
{
	if( !_pSocket->IsValid() )
		return false;

	_sendCallback->Enqueue( buf );

	if( _sendCallback->SetInProgress() )
		return true;

	_sendCallback->SetSession( shared_from_this() );

	if( !_sendCallback->Post() )
	{
		_sendCallback->Clear();
		return false;
	}

	return true;
}