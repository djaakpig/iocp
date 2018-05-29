#include "TcpClientService.h"
#include <MSWSock.h>
#include "Global.h"
#include "IoService.h"
#include "Log.h"
#include "TcpSession.h"
#include "Socket.h"
#include "CircularBuffer.h"

TcpClientService::TcpClientService( const IoService& ioService ) : TcpSessionService( ioService )
{
	_pSocket = new Socket();
}

TcpClientService::~TcpClientService()
{
	SafeDelete( _pSocket );
}

bool TcpClientService::_Start( const SockaddrIn& remoteAddr, const DWORD numReserved )
{
	if( !_pSocket->Create( SOCK_STREAM, IPPROTO_TCP ) )
		return false;

	if( !_extensionTable.Load( _pSocket ) )
		return false;

	auto thisPtr = shared_from_this();

	for( DWORD sessionId = 0; numReserved > sessionId && IsInProgress(); ++sessionId )
	{
		const auto sessionPtr = make_shared<TcpSession>( _extensionTable );

		if( !sessionPtr->Create( thisPtr ) )
			continue;

		if( !sessionPtr->GetSocket()->Bind() )
			continue;

		if( !sessionPtr->GetSocket()->SetNonblock( true ) )
			continue;

		if( !GetIoService().Associate( sessionPtr->GetSocket() ) )
			continue;

		sessionPtr->SetId( sessionId );
		sessionPtr->SetOnConnect( bind( &TcpClientService::_OnConnect, this, placeholders::_1, placeholders::_2 ) );
		sessionPtr->SetOnDisconnect( bind( &TcpClientService::_OnDisconnect, this, placeholders::_1, placeholders::_2 ) );
		sessionPtr->SetOnRecv( bind( &TcpClientService::_OnRecv, this, placeholders::_1, placeholders::_2, placeholders::_3 ) );
		sessionPtr->SetOnSend( bind( &TcpClientService::_OnSend, this, placeholders::_1, placeholders::_2, placeholders::_3 ) );

		if( !sessionPtr->Connect( remoteAddr ) )
			continue;

		_Add( sessionPtr );
	}

	LogForce( "CLIENT start!" );

	return true;
}

void TcpClientService::_Stop()
{
	if( _pSocket )
		_pSocket->Close();

	LogForce( "CLIENT stop!" );
}

bool TcpClientService::_OnConnect( const int e, const shared_ptr<TcpSession>& sessionPtr )
{
	if( e )
	{
		LogError( "connect fail! error:", e );
		return false;
	}

	if( !sessionPtr->GetSocket()->SetOptionNull( SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT ) )
	{
		LogWarning( "UpdateConnectContext fail! id:", sessionPtr->GetId() );
		return false;
	}

	if( !sessionPtr->Recv() )
	{
		LogWarning( "first recv fail! id:", sessionPtr->GetId() );
		return false;
	}

	LogNormal( "connect! id:", sessionPtr->GetId() );

	//	{{SEND_TEST}}
	WSABUF sendBuf;
	sendBuf.buf = new char[10];
	sendBuf.len = 10;
	CopyMemory( sendBuf.buf, "1234567890", 10 );

	sessionPtr->Send( sendBuf );
	//	{{SEND_TEST}}

	return true;
}

bool TcpClientService::_OnDisconnect( const int e, const shared_ptr<TcpSession>& sessionPtr )
{
	_Remove( sessionPtr->GetId() );

	if( e )
	{
		LogError( "disconnect fail! id:", sessionPtr->GetId(), " error:", e );
		return false;
	}

	return true;
}

bool TcpClientService::_OnRecv( const int e, const shared_ptr<TcpSession>& sessionPtr, CircularBuffer& buf )
{
	if( e )
	{
		LogError( "recv fail! id:", sessionPtr->GetId(), " error:", e );
		return false;
	}

	WSABUF wsaBuf;
	while( buf.BeginRead( wsaBuf ) )
	{
		buf.EndRead( wsaBuf.len );

		LogNormal( "recv! id:", sessionPtr->GetId(), " buf:", string( wsaBuf.buf, wsaBuf.len ), ", len:", wsaBuf.len );
	}

	return true;
}

bool TcpClientService::_OnSend( const int e, const shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf )
{
	delete[] buf.buf;

	if( e )
	{
		LogError( "send fail! id:", sessionPtr->GetId(), " error:", e );
		return false;
	}

	return true;
}