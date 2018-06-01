#pragma once
#include <WinSock2.h>

class IoCallback abstract : public OVERLAPPED
{
public:
	IoCallback()
	{
		memset( static_cast<LPOVERLAPPED>( this ), 0, sizeof( OVERLAPPED ) );
	}
	virtual ~IoCallback() = default;

	virtual void Clear()
	{
		memset( static_cast<LPOVERLAPPED>( this ), 0, sizeof( OVERLAPPED ) );
	}
	virtual void OnComplete( const int ) = 0;
};