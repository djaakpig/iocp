#pragma once
#include <WinSock2.h>
#include <atomic>
#include <unordered_map>
#include <memory>
#include "Type.h"
#include "Lock.h"

class CircularBuffer;
class IoService;
class SockaddrIn;
class TcpSession;
class WsaBuf;

class TcpSessionService abstract : public std::enable_shared_from_this<TcpSessionService>
{
	using TcpSessionPtrMap = std::unordered_map<SessionId, std::shared_ptr<TcpSession>>;

public:
	explicit TcpSessionService( const IoService& ioService );
	virtual ~TcpSessionService() = default;

	//	{{GET}}
	inline const IoService& GetIoService() const
	{
		return _ioService;
	}
	inline bool IsInRunning() const
	{
		return _inProgress;
	}
	//	{{GET}}

	void Broadcast( const std::shared_ptr<WsaBuf>& buf );
	std::shared_ptr<TcpSession> Find( const SessionId id );
	bool Start( const SockaddrIn& listenAddr, const DWORD numReserved );
	void Stop();

protected:
	void _Add( const std::shared_ptr<TcpSession>& sessionPtr );
	void _CloseAllSessions();
	void _Remove( const SessionId id );
	void _SetCallbackTo( const std::shared_ptr<TcpSession>& sessionPtr );
	virtual bool _Start( const SockaddrIn& listenAddr, const DWORD numReserved ) = 0;
	virtual void _Stop() = 0;

	//	{{CALLBACK}}
	virtual bool _OnDisconnect( const int e, const std::shared_ptr<TcpSession>& sessionPtr );
	virtual bool _OnPacket( const std::shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf ) = 0;
	bool _OnRecv( const int e, const std::shared_ptr<TcpSession>& sessionPtr, CircularBuffer& buf );
	bool _OnSend( const int e, const std::shared_ptr<TcpSession>& sessionPtr );
	//	{{CALLBACK}}

protected:
	std::shared_mutex _lock;

private:
	const IoService& _ioService;
	std::atomic_bool _inProgress = true;
	TcpSessionPtrMap _sessionMap;
};