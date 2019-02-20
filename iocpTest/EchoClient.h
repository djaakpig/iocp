#pragma once
#include "TcpClientService.h"

class EchoClient : public TcpClientService
{
	using TcpClientService::TcpClientService;

private:
	bool _OnConnect( const int e, const std::shared_ptr<TcpSession>& sessionPtr ) override;
	bool _OnPacket( const std::shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf ) override;
};