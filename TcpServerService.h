#pragma once
#include "TcpSessionService.h"

class CircularBuffer;
class TcpListener;

class TcpServerService final : public TcpSessionService
{
	using TcpSessionService::TcpSessionService;

private:
	bool _Start( const SockaddrIn& listenAddr, const DWORD numReserved ) override;
	void _Stop() override;
	bool _OnAccept( const int e, const shared_ptr<TcpSession>& sessionPtr );
	bool _OnDisconnect( const int e, const shared_ptr<TcpSession>& sessionPtr );
	bool _OnRecv( const int e, const shared_ptr<TcpSession>& sessionPtr, CircularBuffer& buf );
	bool _OnSend( const int e, const shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf );

private:
	shared_ptr<TcpListener> _listenerPtr;
};