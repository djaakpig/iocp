#include "TcpOperation.h"
#include "TcpSession.h"

void TcpOperation::Clear()
{
	__super::Clear();

	_inProgress = false;
	_sessionPtr = nullptr;
}

bool TcpOperation::_HandleError()
{
	const int lastError = WSAGetLastError();
	if( WSA_IO_PENDING == lastError )
		return true;

	return _sessionPtr->PostError( lastError, shared_from_this() );
}