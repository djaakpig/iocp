#include "TcpClientService.h"
#include "WinsockExtension.h"
#include "Log.h"
#include "Global.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "TcpSession.h"

TcpClientService::TcpClientService(const ThreadPool& threadPool) :
	TcpSessionService(threadPool)
{
}

TcpClientService::~TcpClientService()
{
}

bool TcpClientService::_Start(const SockaddrIn& remoteAddr, const uint32_t numReserved)
{
	const auto thisPtr = std::static_pointer_cast<TcpClientService>(shared_from_this());

	for(uint32_t sessionId = 0; numReserved > sessionId && IsInRunning(); ++sessionId)
	{
		const auto session = std::make_shared<TcpSession>(sessionId, _threadPool);

		if(!session->Create())
		{
			continue;
		}

		const auto& sessionSocket = session->GetSocket();

		if(!sessionSocket->Bind())
		{
			continue;
		}

		if(!sessionSocket->LoadExtension())
		{
			return false;
		}

		if(!sessionSocket->SetNonblock(true))
		{
			continue;
		}

		if(!_threadPool.Associate(sessionSocket->GetHandle()))
		{
			continue;
		}

		session->SetOnConnect([thisPtr](const auto e, const auto& session)
		{
			return thisPtr->_OnConnect(e, session);
		});
		_SetCallbackTo(session);

		if(!session->Connect(remoteAddr))
		{
			continue;
		}

		_Add(session);
	}

	return true;
}

void TcpClientService::_Stop()
{
}

bool TcpClientService::_OnConnect(const int32_t e, const std::shared_ptr<TcpSession>& session)
{
	if(e)
	{
		LogError("connect fail! id:", session->GetId(), ", error:", e);
		return false;
	}

	if(!session->GetSocket()->SetOption(SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT))
	{
		LogError("UpdateConnectContext fail! id:", session->GetId());
		return false;
	}

	if(!session->Recv())
	{
		LogError("first recv fail! id:", session->GetId());
		return false;
	}

	LogNormal("connect! id:", session->GetId());

	return true;
}