#pragma once
#include "IoCallbackImpl.hpp"
#include "IoCallbackFn.h"
#include "SockaddrIn.h"

class ExtensionTable;

class IoCallbackConnect final : public IoCallbackImpl<IoCallbackFn>
{
public:
	//	{{SET}}
	inline void SetAddr( const SockaddrIn& addr )
	{
		_addr = addr;
	}
	//	{{SET}}

	void OnComplete( const int e ) override;
	bool Post( const shared_ptr<ExtensionTable>& extensionTablePtr );

private:
	SockaddrIn _addr;
};