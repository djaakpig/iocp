#include "TcpListener.h"
#include "Global.h"
#include "SockaddrIn.h"
#include "Socket.h"
#include "TcpSession.h"

TcpListener::TcpListener()
{
	_socket = std::make_unique<Socket>();
}

bool TcpListener::Create()
{
	if(!_socket->Create(SOCK_STREAM, IPPROTO_TCP))
	{
		return false;
	}

	if(!_socket->SetOptionInt(SOL_SOCKET, SO_REUSEADDR, TRUE))
	{
		return false;
	}

	return true;
}

void TcpListener::Close()
{
	_socket->Close();
}

bool TcpListener::Listen(const SockaddrIn& listenAddr)
{
	if(!_socket->IsValid())
	{
		return false;
	}

	if(!_socket->Bind(listenAddr))
	{
		return false;
	}

	if(SOCKET_ERROR == ::listen(_socket->GetValue(), SOMAXCONN))
	{
		return false;
	}

	return true;
}