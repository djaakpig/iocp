#pragma once
#include "IoCallback.h"

class IoCallbackSend final : public IoCallback
{
public:
	bool OnComplete(const int e, const DWORD numBytes) override;

private:
	char _buf[1024];
};