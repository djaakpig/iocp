#pragma once
#include "TcpServerService.h"

class EchoServer final : public TcpServerService
{
	using TcpServerService::TcpServerService;

private:
	bool _OnPacket(const std::shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf) override;
};