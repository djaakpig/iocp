#pragma once
#include "Type.h"
#include "IoCallbackFn.h"
#include "SockaddrIn.h"
#include <atomic>
using namespace std;

class ExtensionTable;
class IoCallbackAccept;
class IoCallbackConnect;
class IoCallbackDisconnect;
class IoCallbackRecv;
class IoCallbackSend;
class IoCallbackShared;
class Socket;
class TcpListener;
class TcpSessionService;

class TcpSession final : public enable_shared_from_this<TcpSession>
{
public:
	explicit TcpSession( const ExtensionTable& extensionTable );
	~TcpSession();

	//	{{GET}}
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
	inline void SetId( const SessionId id )
	{
		_id = id;
	}
	void SetOnAccept( const IoCallbackFn&& fn );
	void SetOnConnect( const IoCallbackFn&& fn );
	void SetOnDisconnect( const IoCallbackFn&& fn );
	void SetOnRecv( const IoCallbackFnRecv&& fn );
	void SetOnSend( const IoCallbackFnSend&& fn );
	//	{{SET}}

	bool Accept( const shared_ptr<TcpListener>& listenerPtr );
	void Close();
	bool Connect( const SockaddrIn& remoteAddr );
	bool Create( const shared_ptr<TcpSessionService>& servicePtr );
	bool Disconnect();
	bool FillAddr();
	bool PostError( const int e, const shared_ptr<IoCallbackShared>& callbackPtr );
	bool Recv();
	bool Send( const WSABUF& buf );

private:
	shared_ptr<TcpSessionService> _servicePtr;
	const ExtensionTable& _extensionTable;
	Socket* _pSocket = nullptr;
	SessionId _id = 0;
	SockaddrIn _localSockaddr;
	SockaddrIn _remoteSockaddr;
	shared_ptr<IoCallbackAccept> _acceptCallback;
	shared_ptr<IoCallbackConnect> _connectCallback;
	shared_ptr<IoCallbackDisconnect> _disconnectCallback;
	shared_ptr<IoCallbackRecv> _recvCallback;
	shared_ptr<IoCallbackSend> _sendCallback;
};