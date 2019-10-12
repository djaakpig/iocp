#pragma once
#include "Type.h"
#include "SockaddrIn.h"
#include "TcpOperationCallback.h"

class WinsockExtension;
class ThreadPool;
class TcpOperationAccept;
class TcpOperationConnect;
class TcpOperationDisconnect;
class TcpOperationRecv;
class TcpOperationSend;
class TcpOperation;
class Socket;
class TcpListener;
class WsaBuf;

class TcpSession final : public std::enable_shared_from_this<TcpSession>
{
private:
	const SessionId _id = 0;
	const ThreadPool& _threadPool;
	std::unique_ptr<Socket> _socket;
	SockaddrIn _localSockaddr;
	SockaddrIn _remoteSockaddr;
	std::shared_ptr<TcpOperationAccept> _acceptOp;
	std::shared_ptr<TcpOperationConnect> _connectOp;
	std::shared_ptr<TcpOperationDisconnect> _disconnectOp;
	std::shared_ptr<TcpOperationRecv> _recvOp;
	std::shared_ptr<TcpOperationSend> _sendOp;

public:
	TcpSession(const SessionId id, const ThreadPool& threadPool);
	~TcpSession() = default;

	#pragma region getters
	auto GetHandle() const->HANDLE;
	inline SessionId GetId() const
	{
		return _id;
	}
	inline auto GetSocket() const->const std::unique_ptr<Socket>&
	{
		return _socket;
	}
	inline auto GetLocalAddr() const->const SockaddrIn&
	{
		return _localSockaddr;
	}
	inline auto GetRemoteAddr() const->const SockaddrIn&
	{
		return _remoteSockaddr;
	}
	#pragma endregion

	void SetOnAccept(TcpOperationCallback&& callback);
	void SetOnConnect(TcpOperationCallback&& callback);
	void SetOnDisconnect(TcpOperationCallback&& callback);
	void SetOnRecv(TcpOperationCallback&& callback);
	void SetOnSend(TcpOperationCallback&& callback);

	bool Accept(const std::shared_ptr<TcpListener>& listener);
	bool BeginRead(WSABUF& wsaBuf) const;
	void Close();
	bool Connect(const SockaddrIn& remoteAddr);
	bool CopyContextFrom(const std::unique_ptr<Socket>& source) const;
	bool Create();
	bool Disconnect();
	void EndRead(const uint32_t numReadBytes) const;
	void FillAddr();
	bool PostError(const int32_t lastError, const std::shared_ptr<TcpOperation>& operation);
	bool Recv();
	bool Send(const std::shared_ptr<WsaBuf>& buf);
};