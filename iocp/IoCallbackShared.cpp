#include "IoCallbackShared.h"
#include "TcpSession.h"

void IoCallbackShared::Clear()
{
	__super::Clear();

	_inProgress = false;
	_sessionPtr = nullptr;
}

bool IoCallbackShared::_HandleError()
{
	const int lastError = WSAGetLastError();
	if( WSA_IO_PENDING == lastError )
		return true;

	const auto thisPtr = shared_from_this();

	return _sessionPtr->PostError( lastError, thisPtr );
}