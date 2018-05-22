#pragma once
#include "IoCallback.h"
#include <memory>

class TcpSession;

template<class Fn>
class IoCallbackImpl abstract : public IoCallback
{
public:
	void Bind(shared_ptr<TcpSession> sessionPtr, const Fn&& fn)
	{
		_sessionPtr = sessionPtr;
		_fn = fn;
	}

	virtual void Clear() override
	{
		_sessionPtr = nullptr;
	}

protected:
	shared_ptr<TcpSession> _sessionPtr;
	Fn _fn;
};