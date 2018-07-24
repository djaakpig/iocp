#pragma once
#include <WinSock2.h>

class IoOperation abstract : public OVERLAPPED
{
public:
	IoOperation()
	{
		memset( static_cast<LPOVERLAPPED>( this ), 0, sizeof( OVERLAPPED ) );
	}
	virtual ~IoOperation() = default;

	virtual void Clear()
	{
		memset( static_cast<LPOVERLAPPED>( this ), 0, sizeof( OVERLAPPED ) );
	}
	virtual void OnComplete( const int ) = 0;
};