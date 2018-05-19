#pragma once
#include "IoCallback.h"

class IoCallbackSend final : public IoCallback
{
public:
	using Fn = function<bool(const int, shared_ptr<TcpSession>, const WSABUF&)>;

    //  {{SET}}
    inline void Bind(shared_ptr<TcpSession> sessionPtr, const Fn&& fn, const WSABUF& buf)
    {
        _sessionPtr = sessionPtr;
        _fn = fn;
        _buf = buf;
    }
    //  {{SET}}

public:
	bool OnComplete(const int e, const DWORD numBytes) override;

private:
    Fn _fn;
    WSABUF _buf;
    DWORD _numSentBytes = 0;
};