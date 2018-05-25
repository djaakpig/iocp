#include "IoCallback.h"

IoCallback::IoCallback()
{
	Reset();
}

void IoCallback::Clear()
{
	_sessionPtr = nullptr;
	_inProgress = false;
}

void IoCallback::Reset()
{
	SecureZeroMemory(static_cast<LPOVERLAPPED>(this), sizeof(OVERLAPPED));
}