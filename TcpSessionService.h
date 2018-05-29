#pragma once
#include <WinSock2.h>
#include <atomic>
#include <unordered_map>
#include <memory>
#include "Type.h"
#include "Lock.h"

class IoService;
class SockaddrIn;
class TcpSession;

class TcpSessionService abstract : public enable_shared_from_this<TcpSessionService>
{
	using TcpSessionPtrMap = unordered_map<SessionId, shared_ptr<TcpSession>>;

public:
	explicit TcpSessionService( const IoService& ioService );
	virtual ~TcpSessionService() = default;

	//	{{GET}}
	inline bool IsInProgress() const
	{
		return _inProgress;
	}
	inline const IoService& GetIoService() const
	{
		return _ioService;
	}
	//	{{GET}}

	shared_ptr<TcpSession> Find( const SessionId id );
	bool Start( const SockaddrIn& listenAddr, const DWORD numReserved );
	void Stop();

protected:
	void _Add( const shared_ptr<TcpSession>& sessionPtr );
	void _CloseAllSessions();
	void _Remove( const SessionId id );
	virtual bool _Start( const SockaddrIn& listenAddr, const DWORD numReserved ) = 0;
	virtual void _Stop() = 0;

protected:
	shared_mutex _lock;

private:
	const IoService& _ioService;
	TcpSessionPtrMap _sessionMap;
	atomic_bool _inProgress = true;
};