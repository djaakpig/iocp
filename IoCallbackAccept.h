#pragma once
#include "IoCallback.h"

const DWORD LocalSockaddrLen = sizeof(SOCKADDR_IN) + 16;
const DWORD RemoteSockaddrLen = sizeof(SOCKADDR_IN) + 16;

class TcpListener;
class TcpSession;
struct IoCallbackAccept : IoCallback
{
  using Fn = function<bool(const int, shared_ptr<TcpSession>)>;

  bool OnComplete(const int e, const DWORD numBytes) override;
  void Reset() override;

  shared_ptr<TcpListener> listenerPtr;
  shared_ptr<TcpSession> sessionPtr;
  Fn fn;

private:
  char _buf[LocalSockaddrLen + RemoteSockaddrLen];
};