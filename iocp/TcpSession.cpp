#include "TcpSession.h"
#include "Log.h"
#include "WinsockExtension.h"
#include "Global.h"
#include "Socket.h"
#include "IoService.h"
#include "TcpOperationAccept.h"
#include "TcpOperationConnect.h"
#include "TcpOperationDisconnect.h"
#include "TcpOperationError.h"
#include "TcpOperationRecv.h"
#include "TcpOperationSend.h"
#include "TcpListener.h"
#include "TcpSessionService.h"

TcpSession::TcpSession( const SessionId id ) :
	_id( id )
{
	_pSocket = new Socket();
	_acceptOp = make_shared<TcpOperationAccept>();
	_connectOp = make_shared<TcpOperationConnect>();
	_disconnectOp = make_shared<TcpOperationDisconnect>();
	_recvOp = make_shared<TcpOperationRecv>( 15 );
	_sendOp = make_shared<TcpOperationSend>();
}

TcpSession::~TcpSession()
{
	SafeDelete( _pSocket );
}

HANDLE TcpSession::GetHandle() const
{
	return _pSocket->GetHandle();
}

void TcpSession::SetOnAccept( TcpOperationCallback&& callback )
{
	_acceptOp->SetCallback( move( callback ) );
}

void TcpSession::SetOnConnect( TcpOperationCallback&& callback )
{
	_connectOp->SetCallback( move( callback ) );
}

void TcpSession::SetOnDisconnect( TcpOperationCallback&& callback )
{
	_disconnectOp->SetCallback( move( callback ) );
}

void TcpSession::SetOnRecv( TcpOperationCallbackRecv&& callback )
{
	_recvOp->SetCallback( move( callback ) );
}

void TcpSession::SetOnSend( TcpOperationCallback&& callback )
{
	_sendOp->SetCallback( move( callback ) );
}

bool TcpSession::Accept( const shared_ptr<TcpListener>& listenerPtr )
{
	if( !_pSocket->IsValid() )
		return false;

	if( _acceptOp->SetInProgress() )
		return true;

	_acceptOp->SetSession( shared_from_this() );
	_acceptOp->SetListener( listenerPtr );

	_localSockaddr.Clear();
	_remoteSockaddr.Clear();

	if( !_acceptOp->Post( _pSocket->GetExtension() ) )
	{
		_acceptOp->Clear();
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

	if( _connectOp->SetInProgress() )
		return true;

	_connectOp->SetSession( shared_from_this() );
	_connectOp->SetAddr( remoteAddr );

	if( !_connectOp->Post( _pSocket->GetExtension() ) )
	{
		_connectOp->Clear();
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

	if( _disconnectOp->SetInProgress() )
		return true;

	_disconnectOp->SetSession( shared_from_this() );

	if( !_disconnectOp->Post( _pSocket->GetExtension() ) )
	{
		_disconnectOp->Clear();
		return false;
	}

	return true;
}

void TcpSession::FillAddr()
{
	PSOCKADDR pLocalSockaddr = nullptr;
	PSOCKADDR pRemoteSockaddr = nullptr;

	_acceptOp->FillAddrTo( _pSocket->GetExtension(), &pLocalSockaddr, &pRemoteSockaddr );

	_localSockaddr = *pLocalSockaddr;
	_remoteSockaddr = *pRemoteSockaddr;
}

bool TcpSession::PostError( const int lastError, const shared_ptr<TcpOperation>& callbackPtr )
{
	LogError( "Error! id:", GetId(), " e:", lastError );

	if( !_servicePtr )
		return false;

	const auto pErrorCallback = new TcpOperationError();
	pErrorCallback->SetError( lastError );
	pErrorCallback->SetOperation( callbackPtr );

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

	if( _recvOp->SetInProgress() )
		return true;

	_recvOp->SetSession( shared_from_this() );

	if( !_recvOp->Post() )
	{
		_recvOp->Clear();
		return false;
	}

	return true;
}

bool TcpSession::Send( const shared_ptr<WsaBuf>& buf )
{
	if( !_pSocket->IsValid() )
		return false;

	_sendOp->Enqueue( buf );

	if( _sendOp->SetInProgress() )
		return true;

	_sendOp->SetSession( shared_from_this() );

	if( !_sendOp->Post() )
	{
		_sendOp->Clear();
		return false;
	}

	return true;
}