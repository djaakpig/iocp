#include "TcpListener.h"
#include "Socket.h"

TcpListener::TcpListener()
{
  _pSocket=new Socket();
}

TcpListener::~TcpListener()
{
  if(nullptr!=_pSocket)
  {
    delete _pSocket;
    _pSocket=nullptr;
  }
}

HANDLE TcpListener::GetHandle() const
{
  return _pSocket->GetHandle();
}

bool TcpListener::Start(const string& ip,
                        const WORD port,
                        const WORD numInitialSessions)
{
  if(!_pSocket->Create(SOCK_STREAM, IPPROTO_TCP))
    return false;

  if(!_pSocket->SetOptionInt(SOL_SOCKET, SO_REUSEADDR,TRUE))
    return false;

  sockaddr_in listenAddr;
  listenAddr.sin_familly=AF_INET;
  listenAddr.sin_addr.s_addr=inet_addr(ip.c_str());
  listenAddr.sin_port=htons(port);

  if(SOCKET_ERROR==bind(_pSocket->GetSocket(), &listenAddr, sizeof(listenAddr)))
    return false;

  if(SOCKET_ERROR==listen(_pSocket->GetSocket(),SOMAXCONN))
    return false;

  for( int sessionId=0; numInitialSessions>sessionId; ++sessionId)
  {
    const auto pSession=new TcpSession();
    if(pSession->Create())
    {
      const auto r=WSAAccept(_pSocket->GetSocket(), pSession->Get(), ...);
    }
  }

  return true;
}

void TcpListener::Stop()
{
  _pSocket->Close();
}