#pragma once
#include <WinSock2.h>
#include <MSWSock.h>

class Socket;
struct ExtensionTable final
{
	bool Load(const Socket* const pSocket);

	LPFN_TRANSMITFILE transmitFile = nullptr;
	LPFN_ACCEPTEX acceptEx = nullptr;
	LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockaddrs = nullptr;
	LPFN_CONNECTEX connectEx = nullptr;
	LPFN_DISCONNECTEX disconnectEx = nullptr;
};