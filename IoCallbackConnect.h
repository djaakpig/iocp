#pragma once
#include "IoCallbackImpl.hpp"
#include "IoCallbackFn.h"
#include "SockaddrIn.h"

struct ExtensionTable;
class IoCallbackConnect final : public IoCallbackImpl<IoCallbackFn>
{
public:
	//	{{SET}}
	inline void SetAddr( const SockaddrIn& addr )
	{
		_addr = addr;
	}
	//	{{SET}}

	bool OnComplete( const int e ) override;
	bool Post( const ExtensionTable& extensionTable );

private:
	SockaddrIn _addr;
};