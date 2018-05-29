#pragma once
#include "TcpSessionService.h"
#include "ExtensionTable.h"

class CircularBuffer;

class TcpClientService final : public TcpSessionService
{
public:
	explicit TcpClientService( const IoService& ioService );
	virtual ~TcpClientService();

private:
	bool _Start( const SockaddrIn& remoteAddr, const DWORD numReserved ) override;
	void _Stop() override;
	bool _OnConnect( const int e, const shared_ptr<TcpSession>& sessionPtr );
	bool _OnDisconnect( const int e, const shared_ptr<TcpSession>& sessionPtr );
	bool _OnRecv( const int e, const shared_ptr<TcpSession>& sessionPtr, CircularBuffer& buf );
	bool _OnSend( const int e, const shared_ptr<TcpSession>& sessionPtr, const WSABUF& buf );

private:
	Socket* _pSocket = nullptr;
	ExtensionTable _extensionTable;
};