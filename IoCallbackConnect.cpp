#include "IoCallbackConnect.h"

void IoCallbackConnect::Bind(shared_ptr<TcpSession> sessionPtr, const IoCallbackFn&& fn)
{
	_sessionPtr = sessionPtr;
	_fn = fn;
}

bool IoCallbackConnect::OnComplete(const int e, const DWORD)
{
	return _fn(e, _sessionPtr);
}

bool IoCallbackConnect::Post()
{
	return true;
}