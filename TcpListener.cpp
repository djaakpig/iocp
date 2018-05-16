#include "TcpListener.h"
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
	if(nullptr != _pSocket)
	{
		delete _pSocket;
		_pSocket = nullptr;
	}
}

HANDLE TcpListener::GetHandle() const
{
	return _pSocket->GetHandle();
}

bool TcpListener::Start(const string& ip, const WORD port, const WORD numReserved)
{
	if(!_pSocket->Create(SOCK_STREAM, IPPROTO_TCP))
		return false;

	if(!_pSocket->SetOptionInt(SOL_SOCKET, SO_REUSEADDR, TRUE))
		return false;

	sockaddr_in listenAddr;
	listenAddr.sin_familly = AF_INET;
	listenAddr.sin_addr.s_addr = inet_addr(ip.c_str());
	listenAddr.sin_port = htons(port);

	if(SOCKET_ERROR == bind(_pSocket->GetSocketHandle(), &listenAddr, sizeof(listenAddr)))
		return false;

	if(SOCKET_ERROR == listen(_pSocket->GetSocketHandle(), SOMAXCONN))
		return false;

	for(int sessionId = 0; numInitialSessions > sessionId; ++sessionId)
	{
		const auto pSession = new TcpSession();
		//	TODO: pSession, Create(), Accept() 예외처리 하자!
		pSession->Create();
		pSession->GetSocket()->Accept(_pSocket->GetSocketHandle());
	}

	return true;
}

void TcpListener::Stop()
{
	_pSocket->Close();
}