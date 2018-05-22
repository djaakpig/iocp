#pragma once
#include "IoCallback.h"

const DWORD LocalSockaddrLen = sizeof(SOCKADDR_IN) + 16;
const DWORD RemoteSockaddrLen = sizeof(SOCKADDR_IN) + 16;

class Socket;
class TcpListener;
class IoCallbackAccept final : public IoCallback
{
public:
	using Fn = function<bool(const int, shared_ptr<TcpSession>)>;

public:
	//	{{SET}}
    inline void Bind(shared_ptr<TcpListener> listenerPtr, shared_ptr<TcpSession> sessionPtr, const Fn&& fn)
    {
        _listenerPtr = listenerPtr;
        _sessionPtr = sessionPtr;
        _fn = fn;
    }
	//	{{SET}}

	void Clear() override;
	const Socket* GetListenerSocket() const;
	bool OnComplete(const int e, const DWORD numBytes) override;
    bool Post();

private:
	shared_ptr<TcpListener> _listenerPtr;
    Fn _f;
	char _buf[LocalSockaddrLen + RemoteSockaddrLen];
};