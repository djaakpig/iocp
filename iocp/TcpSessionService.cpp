#include "TcpSessionService.h"
#include <MSWSock.h>
#include <algorithm>
#include "CircularBuffer.h"
#include "Log.h"
#include "Socket.h"
#include "TcpSession.h"
using namespace std::chrono;

TcpSessionService::TcpSessionService(const ThreadPool& threadPool) :
	_threadPool{threadPool}
{
}

void TcpSessionService::Broadcast(const std::shared_ptr<WsaBuf>& buf)
{
	const std::shared_lock<std::shared_mutex> s{_lock};

	for_each(_sessionMap.begin(), _sessionMap.end(), [&buf](const auto& sessionPair)
	{
		sessionPair.second->Send(buf);
	});
}

auto TcpSessionService::Find(const SessionId id)->std::shared_ptr<TcpSession>
{
	const std::shared_lock<std::shared_mutex> s{_lock};

	const auto iter = _sessionMap.find(id);
	return _sessionMap.end() != iter ? iter->second : nullptr;
}

bool TcpSessionService::Start(const SockaddrIn& listenAddr, const uint32_t numReserved)
{
	_inProgress = true;

	return _Start(listenAddr, numReserved);
}

void TcpSessionService::Stop()
{
	_inProgress = false;

	_CloseAllSessions();
	_Stop();
}

void TcpSessionService::_Add(const std::shared_ptr<TcpSession>& session)
{
	const std::unique_lock<std::shared_mutex> l{_lock};
	_sessionMap.emplace(std::make_pair(session->GetId(), session));
}

void TcpSessionService::_CloseAllSessions()
{
	DoExclusive(_lock, [this]
	{
		for(const auto& sessionPair : _sessionMap)
		{
			const auto& sessionPtr = sessionPair.second;
			sessionPtr->Disconnect();
			sessionPtr->Close();
		}
	});

	WaitCondition(100ms, [this]
	{
		return !_sessionMap.empty();
	});
}

void TcpSessionService::_Remove(const SessionId id)
{
	const std::unique_lock<std::shared_mutex> l{_lock};
	_sessionMap.erase(id);
}

void TcpSessionService::_SetCallbackTo(const std::shared_ptr<TcpSession>& session)
{
	const auto thisPtr = shared_from_this();
	session->SetOnDisconnect([thisPtr](const auto e, const auto& session)
	{
		return thisPtr->_OnDisconnect(e, session);
	});
	session->SetOnRecv([thisPtr](const auto e, const auto& session)
	{
		return thisPtr->_OnRecv(e, session);
	});
	session->SetOnSend([thisPtr](const auto e, const auto& session)
	{
		return thisPtr->_OnSend(e, session);
	});
}

bool TcpSessionService::_OnDisconnect(const int32_t e, const std::shared_ptr<TcpSession>& session)
{
	LogNormal("onDisconnect! id:", session->GetId());

	_Remove(session->GetId());

	if(e)
	{
		LogError("disconnect fail! id:", session->GetId(), " error:", e);
		return false;
	}

	return true;
}

bool TcpSessionService::_OnRecv(const int32_t e, const std::shared_ptr<TcpSession>& session)
{
	if(e)
	{
		LogError("recv fail! id:", session->GetId(), " error:", e);
		return false;
	}

	WSABUF wsaBuf{0, nullptr};

	while(true)
	{
		wsaBuf.len = sizeof(PacketLength);

		if(!session->BeginRead(wsaBuf))
			break;

		const auto packetLength = *reinterpret_cast<PacketLength*>(wsaBuf.buf);

		if(sizeof(PacketLength) >= packetLength)
		{
			LogError("too small! id:", session->GetId(), " length:", packetLength);
			return false;
		}

		wsaBuf.len = packetLength;

		if(!session->BeginRead(wsaBuf))
			break;

		session->EndRead(wsaBuf.len);

		const WSABUF packet{packetLength - sizeof(PacketLength), wsaBuf.buf + sizeof(PacketLength)};
		if(!_OnPacket(session, packet))
			return false;
	}

	return true;
}

bool TcpSessionService::_OnSend(const int32_t e, const std::shared_ptr<TcpSession>& session)
{
	if(e)
	{
		LogError("send fail! id:", session->GetId(), " error:", e);
		return false;
	}

	return true;
}