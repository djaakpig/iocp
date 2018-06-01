#pragma once
#include <WinSock2.h>
#include <memory>
using namespace std;

class WsaBuf final : public enable_shared_from_this<WsaBuf>
{
public:
	WsaBuf();
	explicit WsaBuf( const DWORD len );
	explicit WsaBuf( const WSABUF& buf );
	WsaBuf( const char* const pBuf, const DWORD len );
	~WsaBuf();

	//	{{GET}}
	inline const WSABUF& Get() const
	{
		return _buf;
	}
	inline WSABUF* operator ->()
	{
		return &_buf;
	}
	inline const WSABUF* operator ->() const
	{
		return &_buf;
	}
	//	{{GET}}

	void CopyFrom( const char* const pBuf, const DWORD len );
	void Move( const DWORD srcOffset, const DWORD dstOffset, const DWORD len );

private:
	void _Clone( const char* const pBuf, const DWORD len );

private:
	WSABUF _buf;
};