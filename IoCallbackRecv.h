#pragma once
#include "IoCallback.h"
#include "CircularBuffer.h"

class IoCallbackRecv final : public IoCallback
{
public:
	explicit IoCallbackRecv( const DWORD capacity );

	//	{{GET}}
	inline CircularBuffer& GetBuffer()
	{
		return _buffer;
	}
	//	{{GET}}

	bool OnComplete(const int e, const DWORD numBytes) override;

private:
	DWORD _Read( const SOCKET s, char* const pBuf, const int sz ) const;

private:
	CircularBuffer _buffer;
};