#include "IoCallbackConnect.h"

bool IoCallbackConnect::OnComplete(const int e, const DWORD numBytes)
{
  return fn(e, sessionPtr);
}

void IoCallbackConnect::Reset()
{
  _Reset();
  sessionPtr = nullptr;
}