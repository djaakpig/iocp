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
    SafeDelete(_pSocket);
}

HANDLE TcpListener::GetHandle() const
{
	return _pSocket->GetHandle();
}

bool TcpListener::ImbueContextTo(Socket* const pSocket) const
{
  auto listenSocket = _pSocket->GetSocketHandle();
  cont auto r = setsockopt(pSocket->GetSocketHandle(),
                            SOL_SOCKET,
                          SO_UPDATE_ACCEPT_CONTEXT,
    static_cast<char*>(&listenSocket),
    sizeof(SOCKET));

  return SOCKET_ERROR != r;
}

bool TcpListener::Start(const string& ip, const WORD port, const WORD numReserved, const IoCallbackAccept::Fn&& fn)
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

    auto thisPtr = shared_from_this();
	for(WORD sessionId = 0; numReserved > sessionId; ++sessionId)
	{
		const auto sessionPtr = make_shared <TcpSession>();
		//	TODO: sessionPtr, Create(), Accept() 예외처리 하자!
		sessionPtr->Create();
		sessionPtr->Accept(thisPtr, fn);
	}

	return true;
}

void TcpListener::Stop()
{
	_pSocket->Close();
}