#pragma once
#include "IoCallbackImpl.hpp"
#include "IoCallbackFn.h"
#include "CircularBuffer.h"

class IoCallbackRecv final : public IoCallbackImpl<IoCallbackFnRecv>
{
public:
	explicit IoCallbackRecv( const DWORD capacity );

	void Clear() override;
	void OnComplete( const int e ) override;
	bool Post();

private:
	bool _OnComplete( const int e );
	pair<int, DWORD> _Read( char* const pBuf, const int sz ) const;

private:
	CircularBuffer _buf;
};