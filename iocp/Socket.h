#pragma once
#include <Winsock2.h>
#include <utility>
#include <memory>

using namespace std;

class SockaddrIn;
class WinsockExtension;

class Socket final
{
public:
	Socket() = default;
	~Socket();

	//	{{GET}}
	inline shared_ptr<WinsockExtension> GetExtension() const
	{
		return _exPtr;
	}
	inline HANDLE GetHandle() const
	{
		return reinterpret_cast<HANDLE>( _socket );
	}
	inline SOCKET GetValue() const
	{
		return _socket;
	}
	inline bool IsValid() const
	{
		return INVALID_SOCKET != _socket;
	}
	//	{{GET}}

	bool Associate( const Socket* const pSocket );
	bool Bind() const;
	bool Bind( const SockaddrIn& addr ) const;
	bool Create( const int type, const int protocol );
	void Close();
	bool LoadExtension();
	inline bool SetNonblock( const bool enable ) const
	{
		u_long val = enable;
		return SOCKET_ERROR != ioctlsocket( _socket, FIONBIO, &val );
	}
	inline bool SetOptionInt( const int level, const int name, const int val ) const
	{
		return SOCKET_ERROR != setsockopt( _socket, level, name, reinterpret_cast<const char*>(&val), sizeof( int ) );
	}
	template<class T>
	inline bool SetOptionPtr( const int level, const int name, const T* const pVal ) const
	{
		return SOCKET_ERROR != setsockopt( _socket, level, name, reinterpret_cast<const char*>(pVal), sizeof( T ) );
	}
	inline bool SetOption( const int level, const int name ) const
	{
		return SOCKET_ERROR != setsockopt( _socket, level, name, nullptr, 0 );
	}

private:
	SOCKET _socket = INVALID_SOCKET;
	shared_ptr<WinsockExtension> _exPtr;
};