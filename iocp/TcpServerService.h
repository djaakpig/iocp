#pragma once
#include "TcpSessionService.h"

class TcpListener;

class TcpServerService abstract : public TcpSessionService
{
	using TcpSessionService::TcpSessionService;

private:
	std::shared_ptr<TcpListener> _listener{};

protected:
	bool _Start(const SockaddrIn& listenAddr, const uint32_t numReserved) override;
	void _Stop() override;

	//	{{CALLBACK}}
	virtual bool _OnAccept(const int32_t e, const std::shared_ptr<TcpSession>& session);
	bool _OnDisconnect(const int32_t e, const std::shared_ptr<TcpSession>& session) override;
	//	{{CALLBACK}}
};