#pragma once
#include "IoCallbackImpl.hpp"
#include "IoCallbackFn.h"
#include "CircularBuffer.h"

class IoCallbackRecv final : public IoCallbackImpl<IoCallbackFnRecv>
{
public:
	explicit IoCallbackRecv( const DWORD capacity );

	bool OnComplete( const int e ) override;
	bool Post();

private:
	bool _OnComplete( const int e );
	pair<int, DWORD> _Read(const SOCKET s, char* const pBuf, const int sz) const;

private:
	CircularBuffer _buffer;
};