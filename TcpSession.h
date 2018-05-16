#pragma once
#include "IoCallback.h"
#include "IIoObject.h"

class Socket;
class TcpSession final : public IIoObject
{
public:
	TcpSession();
	~TcpSession();

	//	{{GET}}
	HANDLE GetHandle() const override;
	inline Socket* GetSocket() const
	{
		return _socket;
	}
	//	{{GET}}

	bool Accept(Socket* const pListen, const IoCallback::Fn&& fn);
	void Close();
	bool Create();
	bool Recv(const IoCallback::Fn&& fn);
	bool Reuse(const IoCallback::Fn&& fn);
	bool Send(const IoCallback::Fn&& fn);

private:
	Socket* _pSocket = nullptr;
	IoCallback _acceptCallback;
	IoCallback _connectCallback;
	IoCallback _disconnectCallback;
	IoCallback _recvCallback;
	IoCallback _sendCallback;
};