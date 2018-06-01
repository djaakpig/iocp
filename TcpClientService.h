#pragma once
#include "TcpSessionService.h"

class Socket;

class TcpClientService final : public TcpSessionService
{
public:
	explicit TcpClientService( const IoService& ioService );
	virtual ~TcpClientService();

private:
	bool _Start( const SockaddrIn& remoteAddr, const DWORD numReserved ) override;
	void _Stop() override;

	//	{{CALLBACK}}
	virtual bool _OnConnect( const int e, const shared_ptr<TcpSession>& sessionPtr );
	bool _OnPacket( const shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf ) override;
	//	{{CALLBACK}}

private:
	Socket* _pSocket = nullptr;
};