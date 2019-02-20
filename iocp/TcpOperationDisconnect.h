#pragma once
#include "TcpOperationImpl.hpp"
#include "TcpOperationCallback.h"

class WinsockExtension;

class TcpOperationDisconnect final : public TcpOperationImpl<TcpOperationCallback>
{
public:
	void OnComplete( const int e ) override;
	bool Post( const std::shared_ptr<WinsockExtension>& exPtr );
};