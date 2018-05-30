#pragma once
#include "IoCallbackFn.h"
#include <string>

class SockaddrIn;
class Socket;
class TcpSessionService;

class TcpListener final : public enable_shared_from_this<TcpListener>
{
public:
	TcpListener();
	~TcpListener();

	//	{{GET}}
	inline Socket* GetSocket() const
	{
		return _pSocket;
	}
	//	{{GET}}

	bool Create();
	void Close();
	bool SetContextTo( const Socket* const pChild ) const;
	bool Listen( const SockaddrIn& listenAddr );

private:
	Socket* _pSocket = nullptr;
};