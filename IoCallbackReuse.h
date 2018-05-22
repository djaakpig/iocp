#pragma once
#include "IoCallback.h"
#include "IoCallbackFn.h"

class IoCallbackReuse final : public IoCallback
{
public:
	void Bind(shared_ptr<TcpSession> sessionPtr, const IoCallbackFn&& fn);
	bool OnComplete(const int e, const DWORD numBytes) override;
	bool Post();

private:
	IoCallbackFn _fn;
};