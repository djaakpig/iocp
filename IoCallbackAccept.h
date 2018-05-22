#pragma once
#include "IoCallback.h"
#include "IoCallbackFn.h"

const DWORD LocalSockaddrLen = sizeof(SOCKADDR_IN) + 16;
const DWORD RemoteSockaddrLen = sizeof(SOCKADDR_IN) + 16;

class Socket;
class TcpListener;
class IoCallbackAccept final : public IoCallback
{
public:
	void Bind(shared_ptr<TcpSession> sessionPtr, const IoCallbackFn&& fn, shared_ptr<TcpListener> listenerPtr);
	void Clear() override;
	const Socket* GetListenerSocket() const;
	bool OnComplete(const int e, const DWORD numBytes) override;
	bool Post();

private:
	shared_ptr<TcpListener> _listenerPtr;
	IoCallbackFn _fn;
	char _buf[LocalSockaddrLen + RemoteSockaddrLen];
};