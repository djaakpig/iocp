#pragma once
#include "TcpOperation.h"

class WinsockExtension;
class TcpListener;

const uint32_t SockaddrLen = sizeof(SOCKADDR_IN) + 16;

class TcpOperationAccept final : public TcpOperation
{
private:
	std::shared_ptr<TcpListener> _listener{};
	char _buf[SockaddrLen * 2];

public:
	inline void SetListener(const std::shared_ptr<TcpListener>& listener)
	{
		_listener = listener;
	}

	void Clear() override;
	void FillAddrTo(const std::shared_ptr<WinsockExtension>& extension,
					PSOCKADDR* const ppRemoteSockaddr,
					PSOCKADDR* const ppLocalSockaddr);
	void OnComplete(const int32_t e) override;
	bool Post(const std::shared_ptr<WinsockExtension>& exPtr);

private:
	bool _OnComplete(const int32_t e);
};