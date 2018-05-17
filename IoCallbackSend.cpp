#include "IoCallbackSend.h"

bool IoCallbackSend::OnComplete(const int e, const DWORD numBytes)
{
  if(e)
    return fn(e, numBytes);

  //TODO: send n bytes.

  return fn(ERROR_SUCCESS, numBytes);
}

void IoCallbackSend::Reset()
{
  _Reset();
  sessionPtr = nullptr;
}