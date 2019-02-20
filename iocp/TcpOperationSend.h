#pragma once
#include "TcpOperationImpl.hpp"
#include "TcpOperationCallback.h"
#include "Lock.h"
#include <list>

class WsaBuf;

class TcpOperationSend final : public TcpOperationImpl<TcpOperationCallback>
{
	using BufferPtrList = std::list<std::shared_ptr<WsaBuf>>;

public:
	void Enqueue( const std::shared_ptr<WsaBuf>& buf );
	void OnComplete( const int e ) override;
	bool Post();

private:
	bool _OnComplete( const int e );
	std::pair<int, DWORD> _Send( char* const pBuf, const int sz ) const;

private:
	std::mutex _lock;
	BufferPtrList _bufs;
	DWORD _numSentBytes = 0;
};