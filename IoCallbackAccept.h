#pragma once
#include "IoCallbackImpl.hpp"
#include "IoCallbackFn.h"

const DWORD SockaddrLen = sizeof(SOCKADDR_IN) + 16;

class ExtensionTable;
class TcpListener;

class IoCallbackAccept final : public IoCallbackImpl<IoCallbackFn>
{
public:
	//	{{SET}}
	inline void SetListener( shared_ptr<TcpListener> listenerPtr )
	{
		_listenerPtr = listenerPtr;
	}
	//	{{SET}}

	void Clear() override;
	void OnComplete( const int e ) override;
	bool Post( const ExtensionTable& extensionTable );

private:
	bool _OnComplete( const int e );

private:
	shared_ptr<TcpListener> _listenerPtr;
	char _buf[SockaddrLen * 2];
};