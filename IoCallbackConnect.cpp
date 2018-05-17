#include "IoCallbackConnect.h"

bool IoCallbackConnect::OnComplete(const int e, const DWORD numBytes)
{
  return _Invoke(e, numBytes);
}