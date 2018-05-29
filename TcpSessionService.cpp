#include "TcpSessionService.h"
#include "TcpSession.h"

TcpSessionService::TcpSessionService( const IoService& ioService ) :
	_ioService( ioService )
{
}

shared_ptr<TcpSession> TcpSessionService::Find( const SessionId id )
{
	return DoShared( _lock, [this, id]()
	{
		const auto iter = _sessionMap.find( id );
		return _sessionMap.end() != iter ? iter->second : nullptr;
	} );
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

void TcpSessionService::_Add( const shared_ptr<TcpSession>& sessionPtr )
{
	DoExclusive( _lock, [this, &sessionPtr]()
	{
		_sessionMap.emplace( make_pair( sessionPtr->GetId(), sessionPtr ) );
	} );
}

void TcpSessionService::_CloseAllSessions()
{
	DoShared( _lock, [this]()
	{
		for( const auto sessionPair : _sessionMap )
			sessionPair.second->Disconnect();
	} );

	WaitCondition( chrono::milliseconds( 100 ), [this]()
	{
		return !_sessionMap.empty();
	} );
}

void TcpSessionService::_Remove( const SessionId id )
{
	DoExclusive( _lock, [this, id]()
	{
		_sessionMap.erase( id );
	} );
}