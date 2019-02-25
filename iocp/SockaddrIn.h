#pragma once
#include <WS2tcpip.h>
#include <Windows.h>
#include <string>

class SockaddrIn final
{
private:
	SOCKADDR_IN _addr;

public:
	SockaddrIn()
	{
		Clear();
		_addr.sin_family = AF_INET;
	}
	SockaddrIn(const std::string& ip, const uint16_t port) : SockaddrIn()
	{
		SetIP(ip);
		SetPort(port);
	}

	inline auto operator = (const SOCKADDR& addr)->const SockaddrIn&
	{
		memcpy(&_addr, &addr, sizeof(SOCKADDR));
		return *this;
	}
	inline auto GetIP() const->std::string
	{
		char ipBuffer[15 + 1];
		return inet_ntop(AF_INET, const_cast<PIN_ADDR>(&_addr.sin_addr), ipBuffer, sizeof(ipBuffer));
	}
	inline auto GetPort() const->uint16_t
	{
		return ntohs(_addr.sin_port);
	}
	inline auto GetSize() const->uint32_t
	{
		return sizeof(SOCKADDR_IN);
	}
	inline auto ToSockAddrPtr()->SOCKADDR*
	{
		return reinterpret_cast<SOCKADDR*>(&_addr);
	}
	inline auto ToSockAddrPtr() const->const SOCKADDR*
	{
		return reinterpret_cast<const SOCKADDR*>(&_addr);
	}
	inline void SetIP(const std::string& ip)
	{
		inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr);
	}
	inline void SetPort(const uint16_t port)
	{
		_addr.sin_port = htons(port);
	}
	inline void Clear()
	{
		memset(&_addr, 0, sizeof(SOCKADDR_IN));
	}
};