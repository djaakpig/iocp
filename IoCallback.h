#pragma once
#include <WinSock2.h>

class IoCallback abstract : public OVERLAPPED
{
public:
	IoCallback()
	{
		SecureZeroMemory( static_cast<LPOVERLAPPED>(this), sizeof( OVERLAPPED ) );
	}
	virtual ~IoCallback() = default;

	virtual void Clear()
	{
		SecureZeroMemory( static_cast<LPOVERLAPPED>(this), sizeof( OVERLAPPED ) );
	}
	virtual void OnComplete( const int ) = 0;
};