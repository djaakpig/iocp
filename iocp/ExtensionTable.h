#pragma once
#include <WinSock2.h>
#include <MSWSock.h>
#include <memory>
using namespace std;

class ExtensionTable final : public enable_shared_from_this<ExtensionTable>
{
public:
	bool Load( SOCKET s );

	LPFN_ACCEPTEX acceptEx = nullptr;
	LPFN_CONNECTEX connectEx = nullptr;
	LPFN_DISCONNECTEX disconnectEx = nullptr;
	LPFN_GETACCEPTEXSOCKADDRS getAcceptExSockaddrs = nullptr;
};