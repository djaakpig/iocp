#pragma once
#include <WS2tcpip.h>
#include <string>
using namespace std;

class SockaddrIn final
{
public:
	SockaddrIn()
	{
		Clear();
		_addr.sin_family = AF_INET;
	}
	SockaddrIn( const string& ip, const WORD port )
	{
		Clear();
		_addr.sin_family = AF_INET;

		SetIP( ip );
		SetPort( port );
	}

	//	{{OPERATOR}}
	inline const SockaddrIn& operator = ( const SOCKADDR& addr )
	{
		CopyMemory( &_addr, &addr, sizeof( SOCKADDR ) );
		return *this;
	}
	//	{{OPERATOR}}

	//	{{GET}}
	inline string GetIP() const
	{
		char ipBuffer[15 + 1];
		return inet_ntop( AF_INET, const_cast<PIN_ADDR>(&_addr.sin_addr), ipBuffer, sizeof( ipBuffer ) );
	}
	inline WORD GetPort() const
	{
		return ntohs( _addr.sin_port );
	}
	inline int GetSize() const
	{
		return sizeof( SOCKADDR_IN );
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

	void Clear()
	{
		SecureZeroMemory( &_addr, sizeof( SOCKADDR_IN ) );
	}

private:
	SOCKADDR_IN _addr;
};