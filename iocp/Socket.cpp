#include "Socket.h"
#include "SockaddrIn.h"
#include "WinsockExtension.h"

Socket::~Socket()
{
	Close();
}

bool Socket::Associate(const Socket& socket)
{
	_extension = socket._extension;
	return nullptr != _extension;
}

bool Socket::Bind() const
{
	static const SockaddrIn addr;
	return Bind(addr);
}

bool Socket::Bind(const SockaddrIn& addr) const
{
	const auto r = ::bind(_socket, addr.ToSockAddrPtr(), addr.GetSize());
	return SOCKET_ERROR != r;
}

bool Socket::Create(const int32_t type, const int32_t protocol)
{
	_socket = WSASocket(AF_INET, type, protocol, nullptr, 0, WSA_FLAG_OVERLAPPED);
	return IsValid();
}

void Socket::Close()
{
	if(IsValid())
	{
		closesocket(_socket);
		_socket = INVALID_SOCKET;
	}
}

bool Socket::LoadExtension()
{
	_extension = std::make_shared<WinsockExtension>();
	return _extension->Load(_socket);
}