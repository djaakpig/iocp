#include "TcpSession.h"
#include "Socket.h"
#include "Global.h"
#include "TcpListener.h"

TcpSession::TcpSession()
{
	_pSocket = new Socket();
}

TcpSession::~TcpSession()
{
	SafeDelete(_pSocket);
}

bool TcpSession::Accept()
{
    return _Accept();
}

bool TcpSession::Accept(shared_ptr<TcpListener> listenerPtr, const IoCallbackAccept::Fn&& fn)
{
    if(!listenerPtr)
        return false;

	//	TODO: 3 초 하드코딩 지우자!
	if(!_pSocket->SetOptionInt(SOL_SOCKET, SO_CONNECT_TIME, 3))
		return false;

    _acceptCallback.listenerPtr = listenerPtr;
    _acceptCallback.sessionPtr = shared_from_this();
	_acceptCallback.fn = fn;

    return _Accept();
}

bool TcpSession::_Accept()
{
    _recvBuf.Reset();

	const auto r = AcceptEx(_acceptCallback.listenerPtr->GetSocket()->GetSocketHandle(),
							_pSocket->GetSocketHandle(),
							_recvBuf.GetPointer(),
							0,
                            LocalSockaddrLen,
                            RemoteSockaddrLen,
							nullptr,
							&_acceptCallback);

	if(SOCKET_ERROR == r)
	{
		const auto lastError = WSAGetLastError();
		if(WSA_IO_PENDING != lastError)
			return false;
	}

	return true;
}

void TcpSession::Close()
{
	_pSocket->Close();
}

bool TcpSession::Create()
{
	return _pSocket->Create(SOCK_STREAM, IPPROTO_TCP);
}

HANDLE TcpSession::GetHandle() const
{
	return _pSocket->GetHandle();
}

bool TcpSession::Recv(const IoCallbackRecv::Fn&& fn)
{
	_recvCallback = fn;

	DWORD flags = 0;
	const auto r = WSARecv(_socket, nullptr, 0, nullptr, &flags, &_recvCallback, nullptr);
	if(FALSE == r)
	{
		const auto lastError = WSAGetLastError();
		if(WSA_IO_PENDING != lastError)
			return false;
	}

	return true;
}

bool TcpSession::Reuse(const IoCallbackReuse::Fn&& fn)
{
	_disconnectCallback = fn;

	const DWORD flags = TF_DISCONNECT | TF_REUSE_SOCKET;
	const auto r = TransmitFile(_socket, nullptr, 0, 0, &_disconnectCallback, nullptr, flags);
	if(FALSE == r)
	{
		const auto lastError = WSAGetLastError();
		if(WSA_IO_PENDING != lastError)
			return false;
	}

	return true;
}

bool TcpSession::Send(const IoCallbackSend::Fn&& fn)
{
	_sendCallback = fn;

	DWORD flags = 0;
	const auto r = WSASend(_socket, nullptr, 0, nullptr, &flags, &_sendCallback, nullptr);
	if(FALSE == r)
	{
		const auto lastError = WSAGetLastError();
		if(WSA_IO_PENDING != lastError)
			return false;
	}

	return true;
}

bool TcpSession::_Accept()
{
    _recvBuf.Reset();

	const auto r = AcceptEx(_listenerPtr->GEtSocket()->GetSocketHandle(),
							_pSocket->GetSocketHandle(),
							_recvBuf.GetPointer(),
							0,
                            LocalSockaddrLen,
                            RemoteSockaddrLen,
							nullptr,
							&_acceptCallback);

	if(SOCKET_ERROR == r)
	{
		const auto lastError = WSAGetLastError();
		if(WSA_IO_PENDING != lastError)
			return false;
	}

	return true;
}