#include "TcpOperation.h"
#include "TcpSession.h"

void TcpOperation::Clear()
{
	__super::Clear();

	_inProgress = false;
	_session = nullptr;
}

bool TcpOperation::_HandleError()
{
	const auto lastError = ::WSAGetLastError();

	if(WSA_IO_PENDING == lastError)
	{
		return true;
	}

	return _session->PostError(lastError, shared_from_this());
}