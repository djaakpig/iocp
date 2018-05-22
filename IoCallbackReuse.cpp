#include "IoCallbackReuse.h"

bool IoCallbackReuse::OnComplete(const int e, const DWORD numBytes)
{
	return _fn(e, _sessionPtr);
}

bool IoCallbackReuse::Post()
{
    const DWORD flags = TF_DISCONNECT | TF_REUSE_SOCKET;
	const auto r = TransmitFile( _listener->GetSocket()->GetSocketHandle(), nullptr, 0, 0, this, nullptr, flags );

	if( !r )
	{
		const auto lastError = WSAGetLastError();
		if( WSA_IO_PENDING != lastError )
			return false;
	}

    return true;
}