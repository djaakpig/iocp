#pragma once
#include "IoCallbackFn.h"
#include "ExtensionTable.h"
#include <string>

class SockaddrIn;
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

	bool Create( const shared_ptr<TcpSessionService>& servicePtr );
	void Close();
	bool ImbueContextTo( const Socket* const pChild ) const;
	bool Listen( const SockaddrIn& listenAddr, const DWORD numReserved, const IoCallbackFn&& fn );

private:
	shared_ptr<TcpSessionService> _servicePtr;
	Socket* _pSocket = nullptr;
	ExtensionTable _extensionTable;
};