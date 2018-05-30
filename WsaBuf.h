#pragma once
#include <WinSock2.h>
#include <memory>
using namespace std;

class WsaBuf final : public enable_shared_from_this<WsaBuf>
{
public:
	WsaBuf();
	explicit WsaBuf( const WSABUF& buf );
	WsaBuf( const char* const pBuf, const DWORD len );
	~WsaBuf();

	//	{{GET}}
	inline const WSABUF& Get() const
	{
		return _buf;
	}
	//	{{GET}}

private:
	void _Clone( const char* const pBuf, const DWORD len );

private:
	WSABUF _buf;
};