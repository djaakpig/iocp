#include "TcpServerService.h"
#include "Log.h"
#include "ThreadPool.h"
#include "Socket.h"
#include "TcpListener.h"
#include "TcpSession.h"

bool TcpServerService::_Start(const SockaddrIn& listenAddr, const uint32_t numReserved)
{
	_listener = std::make_shared<TcpListener>();

	if(!_listener->Create())
		return false;

	const auto& listenSocket = _listener->GetSocket();

	if(!_threadPool.Associate(listenSocket->GetHandle()))
		return false;

	if(!_listener->Listen(listenAddr))
		return false;

	if(!listenSocket->LoadExtension())
		return false;

	const auto acceptCallback = bind(&TcpServerService::_OnAccept, this, std::placeholders::_1, std::placeholders::_2);

	for(uint32_t sessionId = 0; numReserved > sessionId && IsInRunning(); ++sessionId)
	{
		const auto session = std::make_shared<TcpSession>(sessionId, _threadPool);

		if(!session->Create())
			continue;

		if(!session->GetSocket()->Associate(*listenSocket))
			continue;

		session->SetOnAccept(acceptCallback);
		_SetCallbackTo(session);

		if(!session->Accept(_listener))
			session->Close();
	}

	return true;
}

void TcpServerService::_Stop()
{
	_listener->Close();
}

bool TcpServerService::_OnAccept(const int32_t e, const std::shared_ptr<TcpSession>& session)
{
	if(e)
	{
		LogError("accept fail!  id:", session->GetId(), ", error:", e);
		return false;
	}

	if(!IsInRunning())
	{
		LogWarning("refuse! id:", session->GetId());
		return false;
	}

	if(!_threadPool.Associate(session->GetHandle()))
	{
		LogError("accept associate fail! id:", session->GetId());
		return false;
	}

	if(!session->Recv())
	{
		LogError("first recv fail! id:", session->GetId());
		return false;
	}

	_Add(session);

	LogNormal("accept! id:", session->GetId());

	return true;
}

bool TcpServerService::_OnDisconnect(const int32_t e, const std::shared_ptr<TcpSession>& session)
{
	if(!TcpSessionService::_OnDisconnect(e, session))
		return false;

	if(!IsInRunning())
	{
		LogWarning("refuse! id:", session->GetId());
		return false;
	}

	return session->Accept(_listener);
}