#include "IoCallback.h"

IoCallback::IoCallback()
{
	SecureZeroMemory(static_cast<LPOVERLAPPED>(this), sizeof(OVERLAPPED));
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