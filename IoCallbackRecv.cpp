#include "IoCallbackRecv.h"

bool IoCallbackRecv::OnComplete(const int e, const DWORD numBytes)
{
  if(e) return _Invoke(e, numBytes);

  //TODO: recv n bytes.

  return _Invoke(ERROR_SUCCESS, numBytes);
}