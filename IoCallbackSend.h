#pragma once
#include "IoCallback.h"
#include "IoCallbackFn.h"

class IoCallbackSend final : public IoCallback
{
public:
	void Bind(shared_ptr<TcpSession> sessionPtr, const IoCallbackFnSend&& fn, const WSABUF& buf);
	bool OnComplete(const int e, const DWORD numBytes) override;
	bool Post();

private:
	pair<int, DWORD> _Send(const SOCKET s, char* const pBuf, const int sz) const;

private:
	IoCallbackFnSend _fn;
	WSABUF _buf;
	DWORD _numSentBytes = 0;
};