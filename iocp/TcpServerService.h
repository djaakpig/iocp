#pragma once
#include "TcpSessionService.h"

class TcpListener;

class TcpServerService abstract : public TcpSessionService
{
	using TcpSessionService::TcpSessionService;

protected:
	bool _Start( const SockaddrIn& listenAddr, const DWORD numReserved ) override;
	void _Stop() override;

	//	{{CALLBACK}}
	virtual bool _OnAccept( const int e, const std::shared_ptr<TcpSession>& sessionPtr );
	bool _OnDisconnect( const int e, const std::shared_ptr<TcpSession>& sessionPtr ) override;
	//	{{CALLBACK}}

private:
	std::shared_ptr<TcpListener> _listenerPtr;
};