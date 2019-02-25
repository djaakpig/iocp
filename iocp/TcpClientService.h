#pragma once
#include "TcpSessionService.h"

class TcpClientService abstract : public TcpSessionService
{
public:
	explicit TcpClientService(const ThreadPool& threadPool);
	virtual ~TcpClientService();

protected:
	bool _Start(const SockaddrIn& remoteAddr, const uint32_t numReserved) override;
	void _Stop() override;

	//	{{CALLBACK}}
	virtual bool _OnConnect(const int32_t e, const std::shared_ptr<TcpSession>& session);
	//	{{CALLBACK}}
};