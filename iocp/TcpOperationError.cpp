#include "TcpOperationError.h"
#include "TcpOperation.h"

void TcpOperationError::Clear()
{
	__super::Clear();

	delete this;
}

void TcpOperationError::OnComplete( const int )
{
	_operationPtr->OnComplete( _error );
	Clear();
}