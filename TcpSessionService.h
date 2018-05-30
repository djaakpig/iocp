#pragma once
#include <WinSock2.h>
#include <atomic>
#include <unordered_map>
#include <memory>
#include "Type.h"
#include "Lock.h"

class CircularBuffer;
class ExtensionTable;
class IoService;
class SockaddrIn;
class Socket;
class TcpSession;
class WsaBuf;

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

	void Broadcast( const shared_ptr<WsaBuf>& buf );
	shared_ptr<TcpSession> Find( const SessionId id );
	bool Start( const SockaddrIn& listenAddr, const DWORD numReserved );
	void Stop();

protected:
	void _Add( const shared_ptr<TcpSession>& sessionPtr );
	void _CloseAllSessions();
	bool _LoadExtensionTable( const Socket* const pSocket );
	void _Remove( const SessionId id );
	void _SetCallbackTo( const shared_ptr<TcpSession>& sessionPtr );
	virtual bool _Start( const SockaddrIn& listenAddr, const DWORD numReserved ) = 0;
	virtual void _Stop() = 0;

	//	{{CALLBACK}}
	virtual bool _OnConnect( const int e, const shared_ptr<TcpSession>& sessionPtr );
	virtual bool _OnDisconnect( const int e, const shared_ptr<TcpSession>& sessionPtr );
	virtual bool _OnPacket( const shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf ) = 0;
	bool _OnRecv( const int e, const shared_ptr<TcpSession>& sessionPtr, CircularBuffer& buf );
	bool _OnSend( const int e, const shared_ptr<TcpSession>& sessionPtr );
	//	{{CALLBACK}}

protected:
	shared_mutex _lock;
	shared_ptr<ExtensionTable> _extensionTablePtr;

private:
	const IoService& _ioService;
	atomic_bool _inProgress = true;
	TcpSessionPtrMap _sessionMap;
};