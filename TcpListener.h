#pragma once
#include "IIoObject.h"
#include "IoCallbackFn.h"
#include "ExtensionTable.h"
#include <string>

class SockaddrIn;
class Socket;
class IoCallbackAccept;
class TcpListener final : public IIoObject, public enable_shared_from_this<TcpListener>
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
	HANDLE GetHandle() const override;
	bool ImbueContextTo(const Socket* const pChild) const;
	bool Listen(const SockaddrIn& listenAddr, const WORD numReserved, const IoCallbackFn&& fn);

private:
	Socket* _pSocket = nullptr;
	ExtensionTable _extensionTable;
};