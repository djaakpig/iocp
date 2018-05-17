#pragma once
#include "IoCallback.h"

class IoCallbackReuse final : public IoCallback
{
public:
	bool OnComplete(const int e, const DWORD numBytes) override;
};