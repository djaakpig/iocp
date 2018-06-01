#include "TcpServerService.h"
#include "Log.h"
#include "ExtensionTable.h"
#include "IoService.h"
#include "TcpListener.h"
#include "TcpSession.h"
#include "WsaBuf.h"

bool TcpServerService::_Start( const SockaddrIn& listenAddr, const DWORD numReserved )
{
	_listenerPtr = make_shared<TcpListener>();

	if( !_listenerPtr->Create() )
		return false;

	if( !_LoadExtensionTable( _listenerPtr->GetSocket() ) )
		return false;

	if( !GetIoService().Associate( _listenerPtr->GetSocket() ) )
		return false;

	if( !_listenerPtr->Listen( listenAddr ) )
		return false;

	const auto thisPtr = shared_from_this();
	const auto acceptCallback = bind( &TcpServerService::_OnAccept, this, placeholders::_1, placeholders::_2 );

	for( DWORD sessionId = 0; numReserved > sessionId && IsInProgress(); ++sessionId )
	{
		const auto sessionPtr = make_shared<TcpSession>( _extensionTablePtr );

		if( !sessionPtr->Create( thisPtr ) )
			continue;

		sessionPtr->SetId( sessionId );
		sessionPtr->SetOnAccept( move( acceptCallback ) );
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

bool TcpServerService::_OnAccept( const int e, const shared_ptr<TcpSession>& sessionPtr )
{
	if( e )
	{
		LogError( "accept fail!  id:", sessionPtr->GetId(), ", error:", e );
		return false;
	}

	if( !GetIoService().Associate( sessionPtr->GetSocket() ) )
	{
		LogError( "accept associate fail! id:", sessionPtr->GetId() );
		return false;
	}

	if( !sessionPtr->Recv() )
	{
		LogError( "first recv fail! id:", sessionPtr->GetId() );
		return false;
	}

	if( !IsInProgress() )
	{
		LogWarning( "refuse! id:", sessionPtr->GetId() );
		return false;
	}

	_Add( sessionPtr );

	LogNormal( "accept! id:", sessionPtr->GetId() );

	return true;
}

bool TcpServerService::_OnDisconnect( const int e, const shared_ptr<TcpSession>& sessionPtr )
{
	if( !TcpSessionService::_OnDisconnect( e, sessionPtr ) )
		return false;

	if( !IsInProgress() )
	{
		LogWarning( "refuse! id:", sessionPtr->GetId() );
		return false;
	}

	return sessionPtr->Accept( _listenerPtr );
}