#pragma once
#include "IoCallbackImpl.hpp"
#include "IoCallbackFn.h"
#include "Lock.h"
#include <list>

class WsaBuf;

class IoCallbackSend final : public IoCallbackImpl<IoCallbackFn>
{
	using BufferPtrList = list<shared_ptr<WsaBuf>>;

public:
	void Enqueue( const shared_ptr<WsaBuf>& buf );
	void OnComplete( const int e ) override;
	bool Post();

private:
	pair<int, DWORD> _Send( char* const pBuf, const int sz ) const;

private:
	mutex _lock;
	BufferPtrList _bufs;
	DWORD _numSentBytes = 0;
};