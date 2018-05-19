#include "TcpListener.h"
#include "Global.h"
#include "Socket.h"
#include "TcpSession.h"
#include <WS2tcpip.h>
#include <MSWSock.h>

TcpListener::TcpListener()
{
	_pSocket = new Socket();
}

TcpListener::~TcpListener()
{
	SafeDelete(_pSocket);
}

void TcpListener::Close()
{
	_pSocket->Close();
}

bool TcpListener::Create()
{
	if( !_pSocket->Create( SOCK_STREAM, IPPROTO_TCP ) )
		return false;

	if( !_pSocket->SetOptionInt( SOL_SOCKET, SO_REUSEADDR, TRUE ) )
		return false;

	return true;
}

HANDLE TcpListener::GetHandle() const
{
	return _pSocket->GetHandle();
}

bool TcpListener::ImbueContextTo(const Socket* const pChild) const
{
	auto listenSocket = _pSocket->GetSocketHandle();
	return pChild->SetOptionPtr(SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, &listenSocket);
}

bool TcpListener::Start(const string& ip, const WORD port, const WORD numReserved, const IoCallbackAccept::Fn&& fn)
{
	SOCKADDR_IN listenAddr;
	listenAddr.sin_family = AF_INET;
	listenAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &listenAddr.sin_addr);

	if(SOCKET_ERROR == ::bind(_pSocket->GetSocketHandle(), reinterpret_cast<PSOCKADDR>(&listenAddr), sizeof(listenAddr)))
		return false;

	if(SOCKET_ERROR == ::listen(_pSocket->GetSocketHandle(), SOMAXCONN))
		return false;

	auto thisPtr = shared_from_this();
	for(WORD sessionId = 0; numReserved > sessionId; ++sessionId)
	{
		const auto sessionPtr = make_shared<TcpSession>();
		//	TODO: sessionPtr, Create(), Accept() ����ó�� ����!
		sessionPtr->Create();
		sessionPtr->Accept(thisPtr, move(fn));
	}

	return true;
}