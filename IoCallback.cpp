#include "IoCallback.h"

IoCallback::IoCallback()
{
	SecureZeroMemory(static_cast<LPOVERLAPPED>(this), sizeof(OVERLAPPED));
}

void IoCallback::Bind(shared_ptr<TcpSession> sessionPtr, const Fn&& fn)
{
	_sessionPtr = sessionPtr;
	_fn = fn;
}

void IoCallback::Clear()
{
	_sessionPtr = nullptr;
}

void IoCallback::Reset()
{
	SecureZeroMemory(static_cast<LPOVERLAPPED>(this), sizeof(OVERLAPPED));

	_sessionPtr = nullptr;
}

bool IoCallback::_Invoke(const int e, const DWORD numBytes) const
{
	return _fn ? _fn(e, _sessionPtr, numBytes) : true;
}