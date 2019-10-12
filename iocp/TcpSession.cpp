#include "TcpSession.h"
#include "Log.h"
#include "WinsockExtension.h"
#include "Global.h"
#include "Socket.h"
#include "ThreadPool.h"
#include "TcpOperationAccept.h"
#include "TcpOperationConnect.h"
#include "TcpOperationDisconnect.h"
#include "TcpOperationError.h"
#include "TcpOperationRecv.h"
#include "TcpOperationSend.h"
#include "TcpListener.h"

TcpSession::TcpSession(const SessionId id, const ThreadPool& threadPool) :
	_id{id},
	_threadPool{threadPool}
{
	_socket = std::make_unique<Socket>();
	_acceptOp = std::make_shared<TcpOperationAccept>();
	_connectOp = std::make_shared<TcpOperationConnect>();
	_disconnectOp = std::make_shared<TcpOperationDisconnect>();
	_recvOp = std::make_shared<TcpOperationRecv>(15);
	_sendOp = std::make_shared<TcpOperationSend>();
}

auto TcpSession::GetHandle() const->HANDLE
{
	return _socket->GetHandle();
}

void TcpSession::SetOnAccept(TcpOperationCallback&& callback)
{
	_acceptOp->SetCallback(std::forward<TcpOperationCallback>(callback));
}

void TcpSession::SetOnConnect(TcpOperationCallback&& callback)
{
	_connectOp->SetCallback(std::forward<TcpOperationCallback>(callback));
}

void TcpSession::SetOnDisconnect(TcpOperationCallback&& callback)
{
	_disconnectOp->SetCallback(std::forward<TcpOperationCallback>(callback));
}

void TcpSession::SetOnRecv(TcpOperationCallback&& callback)
{
	_recvOp->SetCallback(std::forward<TcpOperationCallback>(callback));
}

void TcpSession::SetOnSend(TcpOperationCallback&& callback)
{
	_sendOp->SetCallback(std::forward<TcpOperationCallback>(callback));
}

bool TcpSession::Accept(const std::shared_ptr<TcpListener>& listener)
{
	if(!_socket->IsValid())
	{
		return false;
	}

	if(_acceptOp->SetInProgress())
	{
		return true;
	}

	_acceptOp->SetSession(shared_from_this());
	_acceptOp->SetListener(listener);

	_localSockaddr.Clear();
	_remoteSockaddr.Clear();

	if(!_acceptOp->Post(_socket->GetExtension()))
	{
		_acceptOp->Clear();

		return false;
	}

	return true;
}

bool TcpSession::BeginRead(WSABUF& wsaBuf) const
{
	return _recvOp->BeginRead(wsaBuf);
}

void TcpSession::Close()
{
	_socket->Close();
}

bool TcpSession::Connect(const SockaddrIn& remoteAddr)
{
	if(!_socket->IsValid())
	{
		return false;
	}

	if(_connectOp->SetInProgress())
	{
		return true;
	}

	_connectOp->SetSession(shared_from_this());
	_connectOp->SetAddr(remoteAddr);

	if(!_connectOp->Post(_socket->GetExtension()))
	{
		_connectOp->Clear();

		return false;
	}

	return true;
}

bool TcpSession::CopyContextFrom(const std::unique_ptr<Socket>& source) const
{
	if(!_socket->SetNonblock(true))
	{
		return false;
	}

	const auto sourceSocket = source->GetValue();

	return _socket->SetOptionPtr(SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, &sourceSocket);
}

bool TcpSession::Create()
{
	if(!_socket->Create(SOCK_STREAM, IPPROTO_TCP))
	{
		return false;
	}

	if(!_socket->UseKeepAlive(5000, 1000))
	{
		return false;
	}

	return true;
}

bool TcpSession::Disconnect()
{
	if(!_socket->IsValid())
	{
		return false;
	}

	if(_disconnectOp->SetInProgress())
	{
		return true;
	}

	_disconnectOp->SetSession(shared_from_this());

	if(!_disconnectOp->Post(_socket->GetExtension()))
	{
		_disconnectOp->Clear();

		return false;
	}

	return true;
}

void TcpSession::EndRead(const uint32_t numReadBytes) const
{
	_recvOp->EndRead(numReadBytes);
}

void TcpSession::FillAddr()
{
	PSOCKADDR pLocalSockaddr = nullptr;
	PSOCKADDR pRemoteSockaddr = nullptr;

	_acceptOp->FillAddrTo(_socket->GetExtension(), &pLocalSockaddr, &pRemoteSockaddr);

	_localSockaddr = *pLocalSockaddr;
	_remoteSockaddr = *pRemoteSockaddr;
}

bool TcpSession::PostError(const int32_t lastError, const std::shared_ptr<TcpOperation>& operation)
{
	LogError("Error! id:", GetId(), " e:", lastError);

	auto errorCallback = std::make_unique<TcpOperationError>();
	errorCallback->SetError(lastError);
	errorCallback->SetOperation(operation);

	if(!_threadPool.Post(errorCallback.get()))
	{
		return false;
	}

	errorCallback.release();

	return true;
}

bool TcpSession::Recv()
{
	if(!_socket->IsValid())
	{
		return false;
	}

	if(_recvOp->SetInProgress())
	{
		return true;
	}

	_recvOp->SetSession(shared_from_this());

	if(!_recvOp->Post())
	{
		_recvOp->Clear();

		return false;
	}

	return true;
}

bool TcpSession::Send(const std::shared_ptr<WsaBuf>& buf)
{
	if(!_socket->IsValid())
	{
		return false;
	}

	_sendOp->Enqueue(buf);

	if(_sendOp->SetInProgress())
	{
		return true;
	}

	_sendOp->SetSession(shared_from_this());

	if(!_sendOp->Post())
	{
		_sendOp->Clear();

		return false;
	}

	return true;
}