#pragma once
#include <Windows.h>
#include <Winsock2.h>
#include <utility>

class Socket final
{
public:
	//	{{GET}}
	inline HANDLE GetHandle() const
	{
		return reinterpret_cast<HANDLE>(_socket);
	}
	inline SOCKET GetSocketHandle() const
	{
		return _socket;
	}
	//	{{GET}}

	bool Create(const int type, const int protocol);
	void Close();
	template<class T>
	inline T GetExtension(GUID&& id) const
	{
		return static_cast<T>(_GetExtension(std::move(id)));
	}
	inline bool SetNonblock(u_long enable)
	{
		return SOCKET_ERROR != ioctlsocket(_socket, FIONBIO, &enable);
	}
	inline bool SetOptionInt(const int level, const int name, int val) const
	{
		return SOCKET_ERROR != setsockopt(_socket, level, name, reinterpret_cast<char*>(&val), sizeof(val));
	}
	template<class T>
	inline bool SetOptionPtr(const int level, const int name, T* const pVal) const
	{
		return SOCKET_ERROR != setsockopt(_socket, level, name, reinterpret_cast<char*>(pVal), sizeof(T));
	}

private:
	LPVOID _GetExtension(GUID&& id) const;

private:
	SOCKET _socket = INVALID_SOCKET;
};