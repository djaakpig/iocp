#pragma once
#include <Winsock2.h>
#include <utility>
#include <memory>

class SockaddrIn;
class WinsockExtension;

class Socket final
{
private:
	SOCKET _socket = INVALID_SOCKET;
	std::shared_ptr<WinsockExtension> _extension{};

public:
	Socket() = default;
	~Socket();

	inline auto GetExtension() const->std::shared_ptr<WinsockExtension>
	{
		return _extension;
	}
	inline auto GetHandle() const->HANDLE
	{
		return reinterpret_cast<HANDLE>(_socket);
	}
	inline auto GetValue() const->SOCKET
	{
		return _socket;
	}
	inline bool IsValid() const
	{
		return INVALID_SOCKET != _socket;
	}

	inline bool SetNonblock(const bool enable) const
	{
		u_long val = enable;
		return SOCKET_ERROR != ioctlsocket(_socket, FIONBIO, &val);
	}
	inline bool SetOptionInt(const int32_t level, const int32_t name, const int32_t val) const
	{
		return SOCKET_ERROR != setsockopt(_socket, level, name, reinterpret_cast<const char*>(&val), sizeof(int32_t));
	}
	template<class T>
	inline bool SetOptionPtr(const int32_t level, const int32_t name, const T* const pVal) const
	{
		return SOCKET_ERROR != setsockopt(_socket, level, name, reinterpret_cast<const char*>(pVal), sizeof(T));
	}
	inline bool SetOption(const int32_t level, const int32_t name) const
	{
		return SOCKET_ERROR != setsockopt(_socket, level, name, nullptr, 0);
	}

	bool Associate(const std::unique_ptr<Socket>& socket);
	bool Bind() const;
	bool Bind(const SockaddrIn& addr) const;
	bool Create(const int32_t type, const int32_t protocol);
	void Close();
	bool LoadExtension();
	bool UseKeepAlive(const uint32_t keepAliveTime, const uint32_t keepAliveInterval) const;
};