#include "TcpSession.h"
#include "Socket.h"
#include "Global.h"
#include "TcpListener.h"
#include <MSWSock.h>

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

bool TcpSession::Accept(shared_ptr<TcpListener> listenerPtr, const IoCallback::Fn&& fn)
{
	if(!listenerPtr) return false;

	//	TODO: 3 초 하드코딩 삭제.
	if(!_pSocket->SetOptionInt(SOL_SOCKET, SO_CONNECT_TIME, 3))
		return false;

	_acceptCallback.Bind(shared_from_this(), move(fn));
	_acceptCallback.SetListener(listenerPtr);

	return _Accept();
}

void TcpSession::Close()
{
	_pSocket->Close();
}

bool TcpSession::Create()
{
	if(!_pSocket->Create(SOCK_STREAM, IPPROTO_TCP))
		return false;

	return true;
}

HANDLE TcpSession::GetHandle() const
{
	return _pSocket->GetHandle();
}

bool TcpSession::Recv(const IoCallback::Fn&& fn)
{
	_recvCallback.Bind(shared_from_this(), move(fn));

	DWORD flags = 0;
	const auto r = WSARecv(_pSocket->GetSocketHandle(), nullptr, 0, nullptr, &flags, &_recvCallback, nullptr);
	if(FALSE == r)
	{
		const auto lastError = WSAGetLastError();
		if(WSA_IO_PENDING != lastError)
			return false;
	}

	return true;
}

bool TcpSession::Reuse(const IoCallback::Fn&& fn)
{
	_reuseCallback.Bind(shared_from_this(), move(fn));

	const DWORD flags = TF_DISCONNECT | TF_REUSE_SOCKET;
	const auto r = TransmitFile(_pSocket->GetSocketHandle(), nullptr, 0, 0, &_reuseCallback, nullptr, flags);
	if(FALSE == r)
	{
		const auto lastError = WSAGetLastError();
		if(WSA_IO_PENDING != lastError)
			return false;
	}

	return true;
}

bool TcpSession::Send(const IoCallback::Fn&& fn)
{
	_sendCallback.Bind(shared_from_this(), move(fn));

	const DWORD flags = 0;
	const auto r = WSASend(_pSocket->GetSocketHandle(), nullptr, 0, nullptr, flags, &_sendCallback, nullptr);
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

	const auto pListenSocket = _acceptCallback.GetListenerSocket();
	const auto r = AcceptEx(pListenSocket->GetSocketHandle(),
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