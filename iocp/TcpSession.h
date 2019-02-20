#pragma once
#include "Type.h"
#include "TcpOperationCallback.h"
#include "SockaddrIn.h"

class WinsockExtension;
class TcpOperationAccept;
class TcpOperationConnect;
class TcpOperationDisconnect;
class TcpOperationRecv;
class TcpOperationSend;
class TcpOperation;
class Socket;
class TcpListener;
class TcpSessionService;
class WinsockExtension;
class WsaBuf;

class TcpSession final : public std::enable_shared_from_this<TcpSession>
{
public:
	explicit TcpSession( const SessionId id );
	~TcpSession();

	//	{{GET}}
	HANDLE GetHandle() const;
	inline SessionId GetId() const
	{
		return _id;
	}
	inline Socket* GetSocket() const
	{
		return _pSocket;
	}
	inline const SockaddrIn& GetLocalAddr() const
	{
		return _localSockaddr;
	}
	inline const SockaddrIn& GetRemoteAddr() const
	{
		return _remoteSockaddr;
	}
	//	{{GET}}

	//	{{SET}}
	void SetOnAccept( TcpOperationCallback&& callback );
	void SetOnConnect( TcpOperationCallback&& callback );
	void SetOnDisconnect( TcpOperationCallback&& callback );
	void SetOnRecv( TcpOperationCallbackRecv&& callback );
	void SetOnSend( TcpOperationCallback&& callback );
	//	{{SET}}

	bool Accept( const std::shared_ptr<TcpListener>& listenerPtr );
	void Close();
	bool Connect( const SockaddrIn& remoteAddr );
	bool Create( const std::shared_ptr<TcpSessionService>& servicePtr );
	bool Disconnect();
	void FillAddr();
	bool PostError( const int lastError, const std::shared_ptr<TcpOperation>& callbackPtr );
	bool Recv();
	bool Send( const std::shared_ptr<WsaBuf>& buf );

private:
	const SessionId _id = 0;
	std::shared_ptr<TcpSessionService> _servicePtr;
	Socket* _pSocket = nullptr;
	SockaddrIn _localSockaddr;
	SockaddrIn _remoteSockaddr;
	std::shared_ptr<TcpOperationAccept> _acceptOp;
	std::shared_ptr<TcpOperationConnect> _connectOp;
	std::shared_ptr<TcpOperationDisconnect> _disconnectOp;
	std::shared_ptr<TcpOperationRecv> _recvOp;
	std::shared_ptr<TcpOperationSend> _sendOp;
};