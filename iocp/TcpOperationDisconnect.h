#pragma once
#include "TcpOperation.h"

class WinsockExtension;

class TcpOperationDisconnect final : public TcpOperation
{
public:
	void OnComplete(const int32_t e) override;
	bool Post(const std::shared_ptr<WinsockExtension>& extension);
};