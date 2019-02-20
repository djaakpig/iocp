#pragma once
#include "TcpOperationImpl.hpp"
#include "TcpOperationCallback.h"
#include "CircularBuffer.h"

class TcpOperationRecv final : public TcpOperationImpl<TcpOperationCallbackRecv>
{
public:
	explicit TcpOperationRecv( const DWORD capacity );

	void Clear() override;
	void OnComplete( const int e ) override;
	bool Post();

private:
	bool _OnComplete( const int e );
	std::pair<int, DWORD> _Read( char* const pBuf, const int sz ) const;

private:
	CircularBuffer _buf;
};