#include "WinsockExtension.h"
#include <stdint.h>

bool WinsockExtension::Load(SOCKET s)
{
	const auto getExt = [s](GUID&& id) -> LPVOID
	{
		LPVOID ptr = nullptr;
		DWORD bytesReturned = 0;
		const auto r = WSAIoctl(s,
								SIO_GET_EXTENSION_FUNCTION_POINTER,
								reinterpret_cast<LPVOID>(&id),
								sizeof(GUID),
								static_cast<LPVOID>(&ptr),
								sizeof(LPVOID),
								&bytesReturned,
								nullptr,
								nullptr);
		return SOCKET_ERROR == r ? nullptr : ptr;
	};

	acceptEx = static_cast<LPFN_ACCEPTEX>(getExt(WSAID_ACCEPTEX));
	if(!acceptEx)
		return false;

	connectEx = static_cast<LPFN_CONNECTEX>(getExt(WSAID_CONNECTEX));
	if(!connectEx)
		return false;

	disconnectEx = static_cast<LPFN_DISCONNECTEX>(getExt(WSAID_DISCONNECTEX));
	if(!disconnectEx)
		return false;

	getAcceptExSockaddrs = static_cast<LPFN_GETACCEPTEXSOCKADDRS>(getExt(WSAID_GETACCEPTEXSOCKADDRS));
	if(!getAcceptExSockaddrs)
		return false;

	return true;
}