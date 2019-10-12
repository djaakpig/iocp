#include "TcpOperationAccept.h"
#include "WinsockExtension.h"
#include "Socket.h"
#include "TcpListener.h"
#include "TcpSession.h"

void TcpOperationAccept::Clear()
{
	_listener = nullptr;

	__super::Clear();
}

void TcpOperationAccept::FillAddrTo(const std::shared_ptr<WinsockExtension>& extension,
									PSOCKADDR* const ppRemoteSockaddr,
									PSOCKADDR* const ppLocalSockaddr)
{
	int32_t localSockaddrLen = 0;
	int32_t remoteSockaddrLen = 0;

	extension->getAcceptExSockaddrs(_buf, 0, SockaddrLen, SockaddrLen,
									ppLocalSockaddr, &localSockaddrLen,
									ppRemoteSockaddr, &remoteSockaddrLen);
}

void TcpOperationAccept::OnComplete(const int32_t e)
{
	const auto r = _OnComplete(e);

	if(!r)
	{
		_session->Disconnect();
	}

	Clear();
}

bool TcpOperationAccept::Post(const std::shared_ptr<WinsockExtension>& extension)
{
	DWORD bytesReceived = 0;
	const auto r = extension->acceptEx(_listener->GetSocket()->GetValue(),
									   _session->GetSocket()->GetValue(),
									   _buf,
									   0,
									   SockaddrLen,
									   SockaddrLen,
									   &bytesReceived,
									   this);

	return r ? true : _HandleError();
}

bool TcpOperationAccept::_OnComplete(const int32_t e)
{
	if(e)
	{
		return _Invoke(e, _session);
	}

	if(!_session->CopyContextFrom(_listener->GetSocket()))
	{
		return _Invoke(WSAGetLastError(), _session);
	}

	_session->FillAddr();

	return _Invoke(ERROR_SUCCESS, _session);
}