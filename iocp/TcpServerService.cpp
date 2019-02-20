#include "TcpServerService.h"
#include "Log.h"
#include "IoService.h"
#include "Socket.h"
#include "TcpListener.h"
#include "TcpSession.h"
#include "WsaBuf.h"

bool TcpServerService::_Start( const SockaddrIn& listenAddr, const DWORD numReserved )
{
	_listenerPtr = std::make_shared<TcpListener>();

	if( !_listenerPtr->Create() )
		return false;

	const auto pListenSocket = _listenerPtr->GetSocket();

	if( !GetIoService().Associate( pListenSocket->GetHandle() ) )
		return false;

	if( !_listenerPtr->Listen( listenAddr ) )
		return false;

	if( !pListenSocket->LoadExtension() )
		return false;

	const auto thisPtr = shared_from_this();
	const auto acceptCallback = bind( &TcpServerService::_OnAccept, this, std::placeholders::_1, std::placeholders::_2 );

	for( DWORD sessionId = 0; numReserved > sessionId && IsInRunning(); ++sessionId )
	{
		const auto sessionPtr = std::make_shared<TcpSession>( sessionId );

		if( !sessionPtr->Create( thisPtr ) )
			continue;

		if( !sessionPtr->GetSocket()->Associate( pListenSocket ) )
			continue;

		sessionPtr->SetOnAccept( acceptCallback );
		_SetCallbackTo( sessionPtr );

		if( !sessionPtr->Accept( _listenerPtr ) )
			sessionPtr->Close();
	}

	return true;
}

void TcpServerService::_Stop()
{
	_listenerPtr->Close();
}

bool TcpServerService::_OnAccept( const int e, const std::shared_ptr<TcpSession>& sessionPtr )
{
	if( e )
	{
		LogError( "accept fail!  id:", sessionPtr->GetId(), ", error:", e );
		return false;
	}

	if( !IsInRunning() )
	{
		LogWarning( "refuse! id:", sessionPtr->GetId() );
		return false;
	}

	if( !GetIoService().Associate( sessionPtr->GetHandle() ) )
	{
		LogError( "accept associate fail! id:", sessionPtr->GetId() );
		return false;
	}

	if( !sessionPtr->Recv() )
	{
		LogError( "first recv fail! id:", sessionPtr->GetId() );
		return false;
	}

	_Add( sessionPtr );

	LogNormal( "accept! id:", sessionPtr->GetId() );

	return true;
}

bool TcpServerService::_OnDisconnect( const int e, const std::shared_ptr<TcpSession>& sessionPtr )
{
	if( !TcpSessionService::_OnDisconnect( e, sessionPtr ) )
		return false;

	if( !IsInRunning() )
	{
		LogWarning( "refuse! id:", sessionPtr->GetId() );
		return false;
	}

	return sessionPtr->Accept( _listenerPtr );
}