#pragma once
#include "TcpOperationImpl.hpp"
#include "TcpOperationCallback.h"
#include "SockaddrIn.h"

class WinsockExtension;

class TcpOperationConnect final : public TcpOperationImpl<TcpOperationCallback>
{
public:
	//	{{SET}}
	inline void SetAddr( const SockaddrIn& addr )
	{
		_addr = addr;
	}
	//	{{SET}}

	void OnComplete( const int e ) override;
	bool Post( const shared_ptr<WinsockExtension>& exPtr );

private:
	SockaddrIn _addr;
};