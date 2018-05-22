#pragma once
#include "IoCallback.h"

class IoCallbackConnect final : public IoCallback
{
public:
	using Fn = function<bool(const int, shared_ptr<TcpSession>>;

public:
    //{{SET}}
    inline void Bind(shared_ptr<TcpSession> sessionPtr, const Fn&& fn)
    {
        _sessionPtr = sessionPtr;
        _fn = fn;
    }
    //{{SET}}

    bool OnComplete(const int e, const DWORD numBytes) override;
    bool Post();

private:
    Fn _fn;
};