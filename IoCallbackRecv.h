#pragma once
#include "IoCallback.h"
#include "CircularBuffer.h"

class IoCallbackRecv final : public IoCallback
{
public:
	using Fn = function<bool(const int, shared_ptr<TcpSession>, CircularBuffer&>;

public:
	explicit IoCallbackRecv( const DWORD capacity );

    //  {{SET}}
    inline void Bind(shared_ptr<TcpSession> sessionPtr, const Fn&& fn)
    {
        _sessionPtr = sessionPtr;
        _fn = fn;
    }
    //  {{SET}}

    bool OnComplete(const int e, const DWORD numBytes) override;
    bool Post();

private:
	pair<int,DWORD> _Read( const SOCKET s, char* const pBuf, const int sz ) const;

private:
    Fn _fn;
	CircularBuffer _buffer;
};