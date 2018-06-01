#pragma once
#include <WinSock2.h>
#include "WsaBuf.h"

class CircularBuffer final
{
public:
	explicit CircularBuffer( const DWORD capacity );

	//	{{GET}}
	inline bool IsNotEnough( const DWORD needBytes ) const
	{
		return _size < needBytes;
	}
	inline bool IsFull() const
	{
		return _buf->len <= _size;
	}
	inline DWORD GetSize() const
	{
		return _size;
	}
	//	{{GET}}

	bool BeginRead( WSABUF& wsaBuf );
	bool BeginWrite( WSABUF& wsaBuf ) const;
	void EndRead( const DWORD numReadBytes );
	void EndWrite( const DWORD numWrittenBytes );
	void Clear();

private:
	void _DoLinearize();

private:
	WsaBuf _buf;
	DWORD _positionToWrite = 0;
	DWORD _positionToRead = 0;
	DWORD _size = 0;
};