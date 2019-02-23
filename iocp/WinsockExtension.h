#pragma once
#include <WinSock2.h>
#include <MSWSock.h>

class WinsockExtension final
{
public:
	LPFN_ACCEPTEX acceptEx = nullptr;
	LPFN_CONNECTEX connectEx = nullptr;
	LPFN_DISCONNECTEX disconnectEx = nullptr;
	LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockaddrs = nullptr;

public:
	bool Load(SOCKET s);
};