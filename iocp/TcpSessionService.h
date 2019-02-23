#pragma once
#include <WinSock2.h>
#include <atomic>
#include <unordered_map>
#include <memory>
#include "Type.h"
#include "Lock.h"

class CircularBuffer;
class ThreadPool;
class SockaddrIn;
class TcpSession;
class WsaBuf;

class TcpSessionService abstract
{
	using TcpSessionPtrMap = std::unordered_map<SessionId, std::shared_ptr<TcpSession>>;

protected:
	const ThreadPool& _threadPool;
	std::shared_mutex _lock;

private:
	std::atomic_bool _inProgress = true;
	TcpSessionPtrMap _sessionMap;

public:
	explicit TcpSessionService(const ThreadPool& threadPool);
	virtual ~TcpSessionService() = default;

	inline bool IsInRunning() const
	{
		return _inProgress;
	}

	void Broadcast(const std::shared_ptr<WsaBuf>& buf);
	auto Find(const SessionId id)->std::shared_ptr<TcpSession>;
	bool Start(const SockaddrIn& listenAddr, const uint32_t numReserved);
	void Stop();

protected:
	void _Add(const std::shared_ptr<TcpSession>& session);
	void _CloseAllSessions();
	void _Remove(const SessionId id);
	void _SetCallbackTo(const std::shared_ptr<TcpSession>& session);
	virtual bool _Start(const SockaddrIn& listenAddr, const uint32_t numReserved) = 0;
	virtual void _Stop() = 0;

	//	{{CALLBACK}}
	virtual bool _OnDisconnect(const int32_t e, const std::shared_ptr<TcpSession>& session);
	virtual bool _OnPacket(const std::shared_ptr<TcpSession>& session, const WSABUF& buf) = 0;
	bool _OnRecv(const int32_t e, const std::shared_ptr<TcpSession>& session);
	bool _OnSend(const int32_t e, const std::shared_ptr<TcpSession>& session);
	//	{{CALLBACK}}
};