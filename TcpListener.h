#pragma once
#include "IIoObject.h"
#include "IoCallbackFn.h"
#include <string>

class Socket;
class TcpListener final : public IIoObject, public enable_shared_from_this<TcpListener>
{
public:
	TcpListener();
	~TcpListener();

	//	{{GET}}
	Socket* GetSocket() const
	{
		return _pSocket;
	}
	//	{{GET}}

	void Close();
	bool Create();
	HANDLE GetHandle() const override;
	bool ImbueContextTo(const Socket* const pChild) const;
	bool Start(const string& ip, const WORD port, const WORD numReserved, const IoCallbackFn&& fn);

private:
	Socket* _pSocket = nullptr;
};