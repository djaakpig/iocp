#pragma once
#include "IIoObject.h"
#include "IoCallbackFn.h"
#include "SockaddrIn.h"
#include <atomic>
using namespace std;

class Socket;
struct ExtensionTable;
class TcpListener;
class IoCallbackAccept;
class IoCallbackConnect;
class IoCallbackDisconnect;
class IoCallbackRecv;
class IoCallbackSend;
class TcpSession final : public IIoObject, public enable_shared_from_this<TcpSession>
{
public:
	explicit TcpSession( const ExtensionTable& extensionTable );
	~TcpSession();

	//	{{GET}}
	HANDLE GetHandle() const override;
	inline uint64_t GetId() const
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
	inline void SetId( const uint64_t id )
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
	bool Create();
	bool Disconnect();
	bool FillAddr();
	bool Recv();
	bool Send( const WSABUF& buf );

private:
	atomic_uint64_t _id = 0;
	Socket* _pSocket = nullptr;
	const ExtensionTable& _extensionTable;
	SockaddrIn _localSockaddr;
	SockaddrIn _remoteSockaddr;
	IoCallbackAccept* _pAcceptCallback = nullptr;
	IoCallbackConnect* _pConnectCallback = nullptr;
	IoCallbackDisconnect* _pDisconnectCallback = nullptr;
	IoCallbackRecv* _pRecvCallback = nullptr;
	IoCallbackSend* _pSendCallback = nullptr;
};