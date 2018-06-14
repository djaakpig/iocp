#pragma once
#include "IoCallbackImpl.hpp"
#include "IoCallbackFn.h"

const DWORD SockaddrLen = sizeof( SOCKADDR_IN ) + 16;

class ExtensionTable;
class TcpListener;

class IoCallbackAccept final : public IoCallbackImpl<IoCallbackFn>
{
public:
	//	{{GET}}
	inline char* GetBuf()
	{
		return _buf;
	}
	inline DWORD GetSize() const
	{
		return sizeof( _buf );
	}
	//	{{GET}}

	//	{{SET}}
	inline void SetListener( const shared_ptr<TcpListener>& listenerPtr )
	{
		_listenerPtr = listenerPtr;
	}
	//	{{SET}}

	void Clear() override;
	void OnComplete( const int e ) override;
	bool Post( const shared_ptr<ExtensionTable>& extensionTablePtr );

private:
	bool _OnComplete( const int e );

private:
	shared_ptr<TcpListener> _listenerPtr;
	char _buf[SockaddrLen * 2];
};