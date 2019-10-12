#pragma once
#include "TcpOperation.h"
#include "SockaddrIn.h"

class WinsockExtension;

class TcpOperationConnect final : public TcpOperation
{
private:
	SockaddrIn _addr;

public:
	#pragma region setters
	inline void SetAddr(const SockaddrIn& addr)
	{
		_addr = addr;
	}
	#pragma endregion

	void OnComplete(const int32_t e) override;
	bool Post(const std::shared_ptr<WinsockExtension>& exPtr);
};