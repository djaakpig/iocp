#pragma once
#include "IoCallback.h"

const DWORD LocalSockaddrLen = sizeof(SOCKADDR_IN) + 16;
const DWORD RemoteSockaddrLen = sizeof(SOCKADDR_IN) + 16;

class Socket;
class TcpListener;
class IoCallbackAccept final : public IoCallback
{
public:
	//	{{SET}}
	inline void SetListener(shared_ptr<TcpListener> listenerPtr)
	{
		_listenerPtr = listenerPtr;
	}
	//	{{SET}}

	void Clear() override;
	const Socket* GetListenerSocket() const;
	bool OnComplete(const int e, const DWORD numBytes) override;

private:
	shared_ptr<TcpListener> _listenerPtr;
	char _buf[LocalSockaddrLen + RemoteSockaddrLen];
};