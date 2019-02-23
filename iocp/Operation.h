#pragma once
#include <stdint.h>
#include <WinSock2.h>

class Operation abstract : public OVERLAPPED
{
public:
	Operation()
	{
		memset(static_cast<LPOVERLAPPED>(this), 0, sizeof(OVERLAPPED));
	}
	virtual ~Operation() = default;

	virtual void Clear()
	{
		memset(static_cast<LPOVERLAPPED>(this), 0, sizeof(OVERLAPPED));
	}
	virtual void OnComplete(const int32_t) = 0;
};