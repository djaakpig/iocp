#include "IoCallbackAccept.h"

bool IoCallbackAccept::OnComplete(const int e, const DWORD numBytes)
{
  if(e)
    return fn(e, sessionPtr);

  if(!listenerPtr->ImbueContextTo(sessionPtr->GetSocket()))
  {
    fn(WSAGetLastError(), sessionPtr);
    return false;
  }

  SOCKADDR_IN localSockaddr, remoteSockaddr;
  int localSockaddr = 0, remoteSockaddrLen = 0;
  GetAcceptExSockaddrs(_buf,
                       0,
                       LocalSockaddrLen,
                       RemoteSockaddrLen,
                       &localSockaddr,
                       &localSockaddrLen,
                       &remoteSockaddr,
                       &remoteSockaddrLen);

  return fn(ERROR_SUCCESS, sessionPtr);
}

void IoCallbackAccept::Reset()
{
  _Reset();
  sessionPtr = nullptr;
}