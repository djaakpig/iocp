#pragma once
#include <Winsock2.h>
#include <utility>

class SockaddrIn;

class Socket final
{
public:
	Socket() = default;
	~Socket();

	//  {{OPERATOR}}
	inline operator HANDLE() const
	{
		return GetHandle();
	}
	inline operator SOCKET() const
	{
		return GetSocketHandle();
	}
	inline operator bool() const
	{
		return IsValid();
	}
	//  {{OPERATOR}}

	//	{{GET}}
	inline HANDLE GetHandle() const
	{
		return reinterpret_cast<HANDLE>( _socket );
	}
	inline SOCKET GetSocketHandle() const
	{
		return _socket;
	}
	inline bool IsValid() const
	{
		return INVALID_SOCKET != _socket;
	}
	//	{{GET}}

	bool Bind() const;
	bool Bind( const SockaddrIn& addr ) const;
	bool Create( const int type, const int protocol );
	void Close();
	inline bool SetNonblock( u_long enable ) const
	{
		return SOCKET_ERROR != ioctlsocket( _socket, FIONBIO, &enable );
	}
	inline bool SetOptionInt( const int level, const int name, int val ) const
	{
		return SOCKET_ERROR != setsockopt( _socket, level, name, reinterpret_cast<char*>(&val), sizeof( int ) );
	}
	template<class T>
	inline bool SetOptionPtr( const int level, const int name, T* const pVal ) const
	{
		return SOCKET_ERROR != setsockopt( _socket, level, name, reinterpret_cast<char*>(pVal), sizeof( T ) );
	}
	inline bool SetOptionNull( const int level, const int name ) const
	{
		return SOCKET_ERROR != setsockopt( _socket, level, name, nullptr, 0 );
	}

private:
	SOCKET _socket = INVALID_SOCKET;
};