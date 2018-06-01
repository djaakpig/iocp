#pragma once
#include "TcpSessionService.h"

class TcpListener;

class TcpServerService final : public TcpSessionService
{
	using TcpSessionService::TcpSessionService;

private:
	bool _Start( const SockaddrIn& listenAddr, const DWORD numReserved ) override;
	void _Stop() override;

	//	{{CALLBACK}}
	virtual bool _OnAccept( const int e, const shared_ptr<TcpSession>& sessionPtr );
	bool _OnDisconnect( const int e, const shared_ptr<TcpSession>& sessionPtr ) override;
	bool _OnPacket( const shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf ) override;
	//	{{CALLBACK}}

private:
	shared_ptr<TcpListener> _listenerPtr;
};