#include "Socket.h"

bool Socket::Create(const int type, const int protocol)
{
	_socket = WSASocket(AF_INET, type, protocol, nullptr, 0, WSA_FLAG_OVERLAPPED);
	return INVALID_SOCKET != _socket;
}

void Socket::Close()
{
	if(INVALID_SOCKET != _socket)
	{
		closesocket(_socket);
		_socket = INVALID_SOCKET;
	}
}

LPVOID Socket::_GetExtension(GUID&& id) const
{
	LPVOID ptr = nullptr;
	DWORD bytesReturned = 0;
	const auto r = WSAIoctl(_socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
							reinterpret_cast<LPVOID>(&id), sizeof(id),
							static_cast<LPVOID>(&ptr), sizeof(ptr),
							&bytesReturned,
							nullptr,
							nullptr);
	if(SOCKET_ERROR == r)
		return nullptr;

	return ptr;
}