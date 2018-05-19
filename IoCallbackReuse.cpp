#include "IoCallbackReuse.h"

bool IoCallbackReuse::OnComplete(const int e, const DWORD numBytes)
{
	return _fn(e, _sessionPtr);
}