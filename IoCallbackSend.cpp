#include "IoCallbackSend.h"

bool IoCallbackSend::OnComplete(const int e, const DWORD numBytes)
{
	if(e) return _Invoke(e, numBytes);

	//TODO: send n bytes.

	return _Invoke(ERROR_SUCCESS, numBytes);
}