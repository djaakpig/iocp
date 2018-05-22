#pragma once
#include "IoCallback.h"
#include "IoCallbackFn.h"
#include "CircularBuffer.h"

class IoCallbackRecv final : public IoCallback
{
public:
	explicit IoCallbackRecv(const DWORD capacity);

	void Bind(shared_ptr<TcpSession> sessionPtr, const IoCallbackFnRecv&& fn);
	bool OnComplete(const int e, const DWORD numBytes) override;
	bool Post();

private:
	pair<int, DWORD> _Read(const SOCKET s, char* const pBuf, const int sz) const;

private:
	IoCallbackFnRecv _fn;
	CircularBuffer _buffer;
};