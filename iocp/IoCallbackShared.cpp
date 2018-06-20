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
    const int lastError = WSAGetLasrError();
	if( WSA_IO_PENDING == lastError )
		return true;

	return _sessionPtr->PostError( lastError, shared_from_this() );
}