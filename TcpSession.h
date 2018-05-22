#pragma once
#include "IIoObject.h"
#include "IoCallbackFn.h"
#include <WinSock2.h>
#include <memory>
using namespace std;

class Socket;
class TcpListener;
class IoCallbackAccept;
class IoCallbackConnect;
class IoCallbackRecv;
class IoCallbackReuse;
class IoCallbackSend;
class TcpSession final : public IIoObject, public enable_shared_from_this<TcpSession>
{
public:
	TcpSession();
	~TcpSession();

	//	{{GET}}
	HANDLE GetHandle() const override;
	inline Socket* GetSocket() const
	{
		return _pSocket;
	}
	//	{{GET}}

	//	{{SET}}
	inline void SetLocalSockaddr(const SOCKADDR& sockaddr)
	{
		_localSockaddr = sockaddr;
	}
	inline void SetRemoteSockaddr(const SOCKADDR& sockaddr)
	{
		_remoteSockaddr = sockaddr;
	}
	//	{{SET}}

	bool Accept();
	bool Accept(const IoCallbackFn&& fn);
	void Close();
	bool Create();
	bool Recv(const IoCallbackFnRecv&& fn);
	bool Reuse(const IoCallbackFn&& fn);
	bool Send(const IoCallbackFnSend&& fn, const WSABUF& buf);

private:
	Socket* _pSocket = nullptr;
	SOCKADDR _localSockaddr;
	SOCKADDR _remoteSockaddr;
	IoCallbackAccept* _pAcceptCallback = nullptr;
	IoCallbackConnect* _pConnectCallback = nullptr;
	IoCallbackRecv* _pRecvCallback = nullptr;
	IoCallbackReuse* _pReuseCallback = nullptr;
	IoCallbackSend* _pSendCallback = nullptr;
};