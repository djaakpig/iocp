#include "TcpSession.h"
#include "Socket.h"
#include "Global.h"
#include "TcpListener.h"
#include <MSWSock.h>
#include "IoCallbackAccept.h"
#include "IoCallbackConnect.h"
#include "IoCallbackRecv.h"
#include "IoCallbackReuse.h"
#include "IoCallbackSend.h"

TcpSession::TcpSession()
{
	_pSocket = new Socket();
	_pAcceptCallback = new IoCallbackAccept();
	_pConnectCallback = new IoCallbackConnect();
	_pRecvCallback = new IoCallbackRecv(1024);
	_pReuseCallback = new IoCallbackReuse();
	_pSendCallback = new IoCallbackSend();
}

TcpSession::~TcpSession()
{
	SafeDelete(_pSocket);
	SafeDelete(_pAcceptCallback);
	SafeDelete(_pConnectCallback);
	SafeDelete(_pRecvCallback);
	SafeDelete(_pReuseCallback);
	SafeDelete(_pSendCallback);
}

HANDLE TcpSession::GetHandle() const
{
	return _pSocket->GetHandle();
}

bool TcpSession::Accept()
{
	SecureZeroMemory(&_localSockaddr, sizeof(_localSockaddr));
	SecureZeroMemory(&_remoteSockaddr, sizeof(_remoteSockaddr));

	return _pAcceptCallback->Post();
}

bool TcpSession::Accept(const IoCallbackFn&& fn, shared_ptr<TcpListener> listenerPtr)
{
	if(!listenerPtr) return false;

	_pAcceptCallback->Bind(shared_from_this(), move(fn), listenerPtr);

	return Accept();
}

void TcpSession::Close()
{
	_pSocket->Close();
}

bool TcpSession::Create()
{
	if(!_pSocket->Create(SOCK_STREAM, IPPROTO_TCP))
		return false;

	if(!_pSocket->SetNonblock(true))
		return false;

	return true;
}

bool TcpSession::Recv(const IoCallbackFnRecv&& fn)
{
	_pRecvCallback->Bind(shared_from_this(), move(fn));

	return _pRecvCallback->Post();
}

bool TcpSession::Reuse(const IoCallbackFn&& fn)
{
	_pReuseCallback->Bind(shared_from_this(), move(fn));

	return _pReuseCallback->Post();
}

bool TcpSession::Send(const IoCallbackFnSend&& fn, const WSABUF& buf)
{
	//  TODO: if the queue is not empty enqueue packet there.

	_pSendCallback->Bind(shared_from_this(), move(fn), buf);

	return _pSendCallback->Post();
}