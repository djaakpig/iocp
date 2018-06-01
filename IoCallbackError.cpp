#include "IoCallbackError.h"
#include "IoCallbackShared.h"

void IoCallbackError::Clear()
{
	__super::Clear();

	delete this;
}

void IoCallbackError::OnComplete( const int )
{
	_callbackPtr->OnComplete( _error );
	Clear();
}