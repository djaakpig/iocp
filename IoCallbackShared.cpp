#include "IoCallbackShared.h"

void IoCallbackShared::Clear()
{
	__super::Clear();

	_inProgress = false;
	_sessionPtr = nullptr;
}