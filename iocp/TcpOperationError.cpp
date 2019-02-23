#include "TcpOperationError.h"
#include "TcpOperation.h"

void TcpOperationError::Clear()
{
	__super::Clear();

	delete this;
}

void TcpOperationError::OnComplete(const int32_t)
{
	_operation->OnComplete(_error);
	Clear();
}