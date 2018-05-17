#pragma once
#include "IIoObject.h"
#include "IoCallbackAccept.h"
#include <string>

class Socket;
class TcpListener final : public IIoObject
{
public:
	TcpListener();
	~TcpListener();

	HANDLE GetHandle() const override;
    bool ImbueContextTo(Socket* const pSocket) const;
	bool Start(const string& ip, const WORD port, const WORD numReserved, const IoCallbackAccept::Fn&& fn);
	void Stop();

private:
	Socket* _pSocket = nullptr;
};