#include "TcpSessionService.h"
#include <MSWSock.h>
#include <algorithm>
#include "CircularBuffer.h"
#include "Log.h"
#include "Socket.h"
#include "TcpSession.h"

TcpSessionService::TcpSessionService( const IoService& ioService ) :
	_ioService( ioService )
{
}

void TcpSessionService::Broadcast( const std::shared_ptr<WsaBuf>& buf )
{
	const std::shared_lock<std::shared_mutex> s( _lock );

	for_each( _sessionMap.begin(), _sessionMap.end(), [&buf]( const auto& sessionPair )
	{
		sessionPair.second->Send( buf );
	});
}

std::shared_ptr<TcpSession> TcpSessionService::Find( const SessionId id )
{
	const std::shared_lock<std::shared_mutex> s( _lock );

	const auto iter = _sessionMap.find( id );
	return _sessionMap.end() != iter ? iter->second : nullptr;
}

bool TcpSessionService::Start( const SockaddrIn& listenAddr, const DWORD numReserved )
{
	_inProgress = true;

	return _Start( listenAddr, numReserved );
}

void TcpSessionService::Stop()
{
	_inProgress = false;

	_CloseAllSessions();
	_Stop();
}

void TcpSessionService::_Add( const std::shared_ptr<TcpSession>& sessionPtr )
{
	const std::unique_lock<std::shared_mutex> l( _lock );
	_sessionMap.emplace( std::make_pair( sessionPtr->GetId(), sessionPtr ) );
}

void TcpSessionService::_CloseAllSessions()
{
	DoExclusive( _lock, [this]
	{
		for( const auto& sessionPair : _sessionMap )
		{
			const auto& sessionPtr = sessionPair.second;
			sessionPtr->Disconnect();
			sessionPtr->Close();
		}
	});

	WaitCondition( std::chrono::milliseconds( 100 ), [this]
	{
		return !_sessionMap.empty();
	});
}

void TcpSessionService::_Remove( const SessionId id )
{
	const std::unique_lock<std::shared_mutex> l( _lock );
	_sessionMap.erase( id );
}

void TcpSessionService::_SetCallbackTo( const std::shared_ptr<TcpSession>& sessionPtr )
{
	sessionPtr->SetOnDisconnect( bind( &TcpSessionService::_OnDisconnect, this, std::placeholders::_1, std::placeholders::_2 ) );
	sessionPtr->SetOnRecv( bind( &TcpSessionService::_OnRecv, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 ) );
	sessionPtr->SetOnSend( bind( &TcpSessionService::_OnSend, this, std::placeholders::_1, std::placeholders::_2 ) );
}

bool TcpSessionService::_OnDisconnect( const int e, const std::shared_ptr<TcpSession>& sessionPtr )
{
	LogNormal( "onDisconnect! id:", sessionPtr->GetId() );

	_Remove( sessionPtr->GetId() );

	if( e )
	{
		LogError( "disconnect fail! id:", sessionPtr->GetId(), " error:", e );
		return false;
	}

	return true;
}

bool TcpSessionService::_OnRecv( const int e, const std::shared_ptr<TcpSession>& sessionPtr, CircularBuffer& buf )
{
	if( e )
	{
		LogError( "recv fail! id:", sessionPtr->GetId(), " error:", e );
		return false;
	}

	WSABUF wsaBuf{ 0, nullptr };

	while( true )
	{
		wsaBuf.len = sizeof( PacketLength );

		if( !buf.BeginRead( wsaBuf ) )
			break;

		const auto packetLength = *reinterpret_cast<PacketLength*>( wsaBuf.buf );

		if( sizeof( PacketLength ) >= packetLength )
		{
			LogError( "too small! id:", sessionPtr->GetId(), " length:", packetLength );
			return false;
		}

		wsaBuf.len = packetLength;

		if( !buf.BeginRead( wsaBuf ) )
			break;

		buf.EndRead( wsaBuf.len );

		const WSABUF packet{ packetLength - sizeof(PacketLength), wsaBuf.buf + sizeof(PacketLength) };
		if( !_OnPacket( sessionPtr, packet ) )
			return false;
	}

	return true;
}

bool TcpSessionService::_OnSend( const int e, const std::shared_ptr<TcpSession>& sessionPtr )
{
	if( e )
	{
		LogError( "send fail! id:", sessionPtr->GetId(), " error:", e );
		return false;
	}

	return true;
}