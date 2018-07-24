#include "Socket.h"
#include "SockaddrIn.h"
#include "WinsockExtension.h"

Socket::~Socket()
{
	Close();
}

bool Socket::Associate( const Socket* const pSocket )
{
	_exPtr = pSocket->_exPtr;
	return nullptr != _exPtr;
}

bool Socket::Bind() const
{
	static const SockaddrIn addr;
	return Bind( addr );
}

bool Socket::Bind( const SockaddrIn& addr ) const
{
	const auto r = ::bind( _socket, addr.ToSockAddrPtr(), addr.GetSize() );
	return SOCKET_ERROR != r;
}

bool Socket::Create( const int type, const int protocol )
{
	_socket = WSASocket( AF_INET, type, protocol, nullptr, 0, WSA_FLAG_OVERLAPPED );
	return IsValid();
}

void Socket::Close()
{
	if( IsValid() )
	{
		closesocket( _socket );
		_socket = INVALID_SOCKET;
	}
}

bool Socket::LoadExtension()
{
	_exPtr = make_shared<WinsockExtension>();

	return _exPtr->Load( _socket );
}