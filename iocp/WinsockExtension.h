#pragma once
#include <WinSock2.h>
#include <MSWSock.h>
using namespace std;

class WinsockExtension final
{
public:
	bool Load( SOCKET s );

	LPFN_ACCEPTEX acceptEx = nullptr;
	LPFN_CONNECTEX connectEx = nullptr;
	LPFN_DISCONNECTEX disconnectEx = nullptr;
	LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockaddrs = nullptr;
};