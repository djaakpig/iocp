#pragma once
#include <Windows.h>
#include <Winsock2.h>

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
	LPVOID GetExtension(const GUID&& id) const;
	bool SetOptionInt(const int level, const int name, int val);

private:
	SOCKET _socket = INVALID_SOCKET;
};