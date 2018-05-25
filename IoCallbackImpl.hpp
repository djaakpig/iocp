#pragma once
#include "IoCallback.h"
#include <memory>

class TcpSession;

template<class Fn>
class IoCallbackImpl abstract : public IoCallback
{
public:
	//	{{SET}}
	inline void SetFn( const Fn&& fn )
	{
		_fn = fn;
	}
	//	{{SET}}

protected:
	template<class... Args>
	inline bool _Invoke( Args&&... args )
	{
		return _fn ? _fn( forward<Args>( args )... ) : false;
	}

private:
	Fn _fn;
};