#include "IoCallbackReuse.h"

bool IoCallbackReuse::OnComplete(const int e, const DWORD numBytes)
{
  if(e)
    return fn(e, sessionPtr);

  return fn(ERROR_SUCCESS, sessionPtr);
}

void IoCallbackReuse::Reset()
{
  _Reset();
  sessionPtr = nullptr;
}