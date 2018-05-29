#pragma once
#include "IoCallbackImpl.hpp"
#include "IoCallbackFn.h"
#include "Lock.h"
#include <list>

class IoCallbackSend final : public IoCallbackImpl<IoCallbackFnSend>
{
public:
	void Enqueue( const WSABUF& buf );
	void OnComplete( const int e ) override;
	bool Post();

private:
	pair<int, DWORD> _Send( const SOCKET s, char* const pBuf, const int sz ) const;

private:
	mutex _lock;
	list<WSABUF> _bufs;
	DWORD _numSentBytes = 0;
};