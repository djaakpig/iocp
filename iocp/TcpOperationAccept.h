#pragma once
#include "TcpOperationImpl.hpp"
#include "TcpOperationCallback.h"

const DWORD SockaddrLen = sizeof( SOCKADDR_IN ) + 16;

class WinsockExtension;
class TcpListener;

class TcpOperationAccept final : public TcpOperationImpl<TcpOperationCallback>
{
public:
	//	{{GET}}
	inline DWORD GetSize() const
	{
		return sizeof( _buf );
	}
	//	{{GET}}

	//	{{SET}}
	inline void SetListener( const std::shared_ptr<TcpListener>& listenerPtr )
	{
		_listenerPtr = listenerPtr;
	}
	//	{{SET}}

	void Clear() override;
	void FillAddrTo( const std::shared_ptr<WinsockExtension>& exPtr,
					 PSOCKADDR* const ppRemoteSockaddr,
					 PSOCKADDR* const ppLocalSockaddr );
	void OnComplete( const int e ) override;
	bool Post( const std::shared_ptr<WinsockExtension>& exPtr );

private:
	bool _OnComplete( const int e );

private:
	std::shared_ptr<TcpListener> _listenerPtr;
	char _buf[SockaddrLen * 2];
};