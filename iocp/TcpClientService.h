#pragma once
#include "TcpSessionService.h"

class Socket;

class TcpClientService abstract : public TcpSessionService
{
public:
	explicit TcpClientService( const IoService& ioService );
	virtual ~TcpClientService();

protected:
	bool _Start( const SockaddrIn& remoteAddr, const DWORD numReserved ) override;
	void _Stop() override;

	//	{{CALLBACK}}
	virtual bool _OnConnect( const int e, const std::shared_ptr<TcpSession>& sessionPtr );
	//	{{CALLBACK}}

private:
	Socket* _pSocket = nullptr;
};