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

class TcpSession final : public enable_shared_from_this<TcpSession>
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
	void SetOnAccept( const TcpOperationCallback&& callback );
	void SetOnConnect( const TcpOperationCallback&& callback );
	void SetOnDisconnect( const TcpOperationCallback&& callback );
	void SetOnRecv( const TcpOperationCallbackRecv&& callback );
	void SetOnSend( const TcpOperationCallback&& callback );
	//	{{SET}}

	bool Accept( const shared_ptr<TcpListener>& listenerPtr );
	void Close();
	bool Connect( const SockaddrIn& remoteAddr );
	bool Create( const shared_ptr<TcpSessionService>& servicePtr );
	bool Disconnect();
	void FillAddr();
	bool PostError( const int lastError, const shared_ptr<TcpOperation>& callbackPtr );
	bool Recv();
	bool Send( const shared_ptr<WsaBuf>& buf );

private:
	const SessionId _id = 0;
	shared_ptr<TcpSessionService> _servicePtr;
	Socket* _pSocket = nullptr;
	SockaddrIn _localSockaddr;
	SockaddrIn _remoteSockaddr;
	shared_ptr<TcpOperationAccept> _acceptOp;
	shared_ptr<TcpOperationConnect> _connectOp;
	shared_ptr<TcpOperationDisconnect> _disconnectOp;
	shared_ptr<TcpOperationRecv> _recvOp;
	shared_ptr<TcpOperationSend> _sendOp;
};