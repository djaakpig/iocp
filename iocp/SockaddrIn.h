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

	inline const SockaddrIn& operator = (const SOCKADDR& addr)
	{
		memcpy(&_addr, &addr, sizeof(SOCKADDR));
		return *this;
	}
	inline std::string GetIP() const
	{
		char ipBuffer[15 + 1];
		return inet_ntop(AF_INET, const_cast<PIN_ADDR>(&_addr.sin_addr), ipBuffer, sizeof(ipBuffer));
	}
	inline uint16_t GetPort() const
	{
		return ntohs(_addr.sin_port);
	}
	inline uint32_t GetSize() const
	{
		return sizeof(SOCKADDR_IN);
	}
	inline SOCKADDR* ToSockAddrPtr()
	{
		return reinterpret_cast<SOCKADDR*>(&_addr);
	}
	inline const SOCKADDR* ToSockAddrPtr() const
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