#include "IoCallbackSend.h"

bool IoCallbackSend::OnComplete(const int e, const DWORD numBytes)
{
	if(e) return _fn(e, _sessionPtr, _buf);

    _numSentBytes += numBytes;

    if(_numSentBytes >= _buf.len)
        return _fn(ERROR_SUCCESS, _sessionPtr, _buf);

    const auto s = _sessionPtr->GetSocket()->GetSocketHandle();
    WSABUF buf {_buf.buf + _numSentBytes, _buf.len - _numSentBytes };
    const r = WSASend( s, &buf, 1, ... );
    if(!r)
    {
        const lastError = WSAGetLastError();
        if(WSA_IO_PENDING != lastError)
            return false;
    }

    return true;
}