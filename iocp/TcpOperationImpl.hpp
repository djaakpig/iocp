#pragma once
#include "TcpOperation.h"
#include <memory>

template<class Callback>
class TcpOperationImpl abstract : public TcpOperation
{
public:
	//	{{SET}}
	inline void SetCallback( const Callback&& callback )
	{
		_callback = callback;
	}
	//	{{SET}}

protected:
	template<class... Args>
	inline bool _Invoke( Args&&... args )
	{
		return _callback ? _callback( forward<Args>( args )... ) : false;
	}

private:
	Callback _callback;
};