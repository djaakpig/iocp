#include "TcpClientService.h"
#include "WinsockExtension.h"
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

	auto thisPtr = shared_from_this();
	const auto connectCallback = bind( &TcpClientService::_OnConnect, this, std::placeholders::_1, std::placeholders::_2 );

	for( DWORD sessionId = 0; numReserved > sessionId && IsInRunning(); ++sessionId )
	{
		const auto sessionPtr = std::make_shared<TcpSession>( sessionId );
		if( !sessionPtr->Create( thisPtr ) )
			continue;

		const auto pSessionSocket = sessionPtr->GetSocket();
		if( !pSessionSocket->Bind() )
			continue;

		if( !pSessionSocket->LoadExtension() )
			return false;

		if( !pSessionSocket->SetNonblock( true ) )
			continue;

		if( !GetIoService().Associate( pSessionSocket->GetHandle() ) )
			continue;

		sessionPtr->SetOnConnect( connectCallback );
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

bool TcpClientService::_OnConnect( const int e, const std::shared_ptr<TcpSession>& sessionPtr )
{
	if( e )
	{
		LogError( "connect fail! id:", sessionPtr->GetId(), ", error:", e );
		return false;
	}

	if( !sessionPtr->GetSocket()->SetOption( SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT ) )
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

	return true;
}