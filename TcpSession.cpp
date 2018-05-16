#include "TcpSession.h"
#include "Socket.h"
#include "Global.h"

TcpSession::TcpSession()
{
	_pSocket = new Socket();
}

TcpSession::~TcpSession()
{
	SafeDelete(_pSocket);
}

bool TcpSession::Accept(Socket* const pListen, const IoCallback::Fn&& fn)
{
	//	TODO: 3 초 하드코딩 지우자!
	if(!_pSocket->SetOptionInt(SOL_SOCKET, SO_CONNECT_TIME, 3))
		return false;

	_recvBuf.Reset();
	_acceptCallback = fn;

	const auto r = AcceptEx(pListen->GetSocketHandle(),
							_pSocket->GetSocketHandle(),
							_recvBuf.GetPointer(),
							0,
							sizeof(SOCKADDR_IN) + 16,
							sizeof(SOCKADDR_IN) + 16,
							nullptr,
							&_acceptCallback);

	if(SOCKET_ERROR == r)
	{
		const auto lastError = WSAGetLastError();
		if(WSA_IO_PENDING != lastError)
		{
			return false;
		}
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

bool TcpSession::Recv(const IoCallback::Fn&& fn)
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

bool TcpSession::Reuse(const IoCallback::Fn&& fn)
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

bool TcpSession::Send(const IoCallback::Fn&& fn)
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