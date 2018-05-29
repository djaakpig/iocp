#include "IoCallbackError.h"
#include "IoCallbackShared.h"

void IoCallbackError::Clear()
{
	_callbackPtr->Clear();
	_callbackPtr = nullptr;

	__super::Clear();

	delete this;
}

void IoCallbackError::OnComplete( const int )
{
	_callbackPtr->OnComplete( _error );
	Clear();
}