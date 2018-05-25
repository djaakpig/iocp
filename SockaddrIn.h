#pragma once
#include <WS2tcpip.h>
#include <string>
using namespace std;

class SockaddrIn final
{
public:
	SockaddrIn()
	{
		SecureZeroMemory( &_addr, sizeof( _addr ) );
		_addr.sin_family = AF_INET;
	}
	SockaddrIn(const string& ip, const WORD port)
	{
		SecureZeroMemory( &_addr, sizeof( _addr ) );
		_addr.sin_family = AF_INET;

		SetIP( ip );
		SetPort( port );
	}

	//	{{GET}}
	inline string GetIP() const
	{
		char ipBuffer[15 + 1];
		return inet_ntop( AF_INET, const_cast<PSOCKADDR_IN>(&_addr), ipBuffer, sizeof ipBuffer );
	}
	inline WORD GetPort() const
	{
		return ntohs( _addr.sin_port );
	}
	inline int GetSize() const
	{
		return sizeof _addr;
	}
	inline SOCKADDR* ToSockAddrPtr()
	{
		return reinterpret_cast<SOCKADDR*>(&_addr);
	}
	inline const SOCKADDR* ToSockAddrPtr() const
	{
		return reinterpret_cast<const SOCKADDR*>(&_addr);
	}
	//	{{GET}}

	//	{{SET}}
	inline void SetIP( const string& ip )
	{
		inet_pton( AF_INET, ip.c_str(), &_addr.sin_addr );
	}
	inline void SetPort( const WORD port )
	{
		_addr.sin_port = htons( port );
	}
	//	{{SET}}

private:
	SOCKADDR_IN _addr;
};