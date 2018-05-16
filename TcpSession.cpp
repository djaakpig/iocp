#include "TcpSession.h"
#include "Socket.h"

TcpSession::TcpSession()
{
  _pSocket=new Socket();
}

TcpSession::~TcpSession()
{
  if(nullptr!=_pSocket)
  {
    delete _pSocket;
    _pSocket=nullptr;
  }
}

void TcpSession::Close()
{
  _pSocket->Close();
}

bool TcpSession::Create()
{
  return _pSocket->Create(SOCK_STREAM, IPPROTO_TCP);
}

HANDLE TcpSession::GetHandle() const
{
  return _pSocket->GetHandle();
}

bool TcpSession::Recv()
{
  return true;
}

bool TcpSession::Send()
{
  return true;
}