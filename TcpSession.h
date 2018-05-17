#pragma once
#include "IIoObject.h"
#include "IoCallbackAccept.h"
#include "IoCallbackConnect.h"
#include "IoCallbackDisconnect.h"
#include "IoCallbackRecv.h"
#include "IoCallbackSend.h"

class Socket;
class TcpListener;
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

    bool Accept();
	bool Accept(shared_ptr<TcpListener> listenerPtr, const IoCallbackAccept::Fn&& fn);
	void Close();
	bool Create();
	bool Recv(const IoCallbackRecv::Fn&& fn);
	bool Reuse(const IoCallbackReuse::Fn&& fn);
	bool Send(const IoCallbackSend::Fn&& fn);

private:
    bool _Accept();

private:
	Socket* _pSocket = nullptr;
	IoCallbackAccept _acceptCallback;
	IoCallbackConnect _connectCallback;
	IoCallbackRecv _recvCallback;
	IoCallbackReuse _reuseCallback;
	IoCallbackSend _sendCallback;
};