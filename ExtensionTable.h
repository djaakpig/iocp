#pragma once
#include <WinSock2.h>
#include <MSWSock.h>

class Socket;
struct ExtensionTable final
{
	bool Load(const Socket* const pSocket);

	LPFN_TRANSMITFILE TransmitFile = nullptr;
	LPFN_ACCEPTEX AcceptEx = nullptr;
	LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs = nullptr;
	LPFN_CONNECTEX ConnectEx = nullptr;
	LPFN_DISCONNECTEX DisconnectEx = nullptr;
};