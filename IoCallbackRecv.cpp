#include "IoCallbackRecv.h"

bool IoCallbackRecv::OnComplete(const int e, const DWORD numBytes)
{
  if(e)
    return fn(e, numBytes);

  //TODO: recv n bytes.

  return fn(ERROR_SUCCESS, numBytes);
}

void IoCallbackRecv::Reset()
{
  _Reset();
  sessionPtr = nullptr;
}