#pragma once
#include "IoCallback.h"
#include <memory>
using namespace std;

class IoCallbackShared;

class IoCallbackError final : public IoCallback
{
public:
	//	{{SET}}
	inline void SetError( const int e )
	{
		_error = e;
	}
	inline void SetCallback( const shared_ptr<IoCallbackShared>& callbackPtr )
	{
		_callbackPtr = callbackPtr;
	}
	//	{{SET}}

	void Clear() override;
	void OnComplete( const int ) override;

private:
	int _error = ERROR_SUCCESS;
	shared_ptr<IoCallbackShared> _callbackPtr;
};