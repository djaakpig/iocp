#include "TcpClientService.h"
#include "ExtensionTable.h"
#include "Log.h"
#include "Global.h"
#include "Socket.h"
#include "IoService.h"
#include "TcpSession.h"
#include "WsaBuf.h"

TcpClientService::TcpClientService( const IoService& ioService ) :
	TcpSessionService( ioService )
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

	if( !_LoadExtensionTable( _pSocket ) )
		return false;

	auto thisPtr = shared_from_this();

	for( DWORD sessionId = 0; numReserved > sessionId && IsInProgress(); ++sessionId )
	{
		const auto sessionPtr = make_shared<TcpSession>( _extensionTablePtr );

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
		_SetCallbackTo( sessionPtr );

		if( !sessionPtr->Connect( remoteAddr ) )
			continue;

		_Add( sessionPtr );
	}

	return true;
}

void TcpClientService::_Stop()
{
	if( _pSocket )
		_pSocket->Close();
}

bool TcpClientService::_OnConnect( const int e, const shared_ptr<TcpSession>& sessionPtr )
{
	if( e )
	{
		LogError( "connect fail! id:", sessionPtr->GetId(), ", error:", e );
		return false;
	}

	if( !sessionPtr->GetSocket()->SetOptionNull( SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT ) )
	{
		LogError( "UpdateConnectContext fail! id:", sessionPtr->GetId() );
		return false;
	}

	if( !sessionPtr->Recv() )
	{
		LogError( "first recv fail! id:", sessionPtr->GetId() );
		return false;
	}

	LogNormal( "connect! id:", sessionPtr->GetId() );

	//	{{ECHO_TEST}}
	sessionPtr->Send( make_shared<WsaBuf>( "1234567890", 10 ) );
	//	{{ECHO_TEST}}

	return true;
}

bool TcpClientService::_OnPacket( const shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf )
{
	LogNormal( "received packet! id:", sessionPtr->GetId(), ", len:", buf.len, ", msg:", string( buf.buf, buf.len ) );

	//	{{ECHO_TEST}}
	sessionPtr->Send( make_shared<WsaBuf>( buf ) );
	//	{{ECHO_TEST}}

	return true;
}