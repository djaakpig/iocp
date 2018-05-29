#pragma once
#include <WinSock2.h>
#include <MSWSock.h>

class Socket;

class ExtensionTable final
{
public:
	bool Load( const Socket* const pSocket );

	LPFN_ACCEPTEX acceptEx = nullptr;
	LPFN_CONNECTEX connectEx = nullptr;
	LPFN_DISCONNECTEX disconnectEx = nullptr;
	LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockaddrs = nullptr;
};