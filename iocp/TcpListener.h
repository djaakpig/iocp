#pragma once
#include <memory>

class SockaddrIn;
class Socket;

class TcpListener final
{
private:
	std::unique_ptr<Socket> _socket;

public:
	TcpListener();
	~TcpListener();

	inline auto GetSocket() const->const std::unique_ptr<Socket>&
	{
		return _socket;
	}

	bool Create();
	void Close();
	bool Listen(const SockaddrIn& listenAddr);
};