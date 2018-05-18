#pragma once
#include <Windows.h>
#include <WinSock2.h>

class WinsockStarter final
{
public:
	explicit WinsockStarter(const BYTE highVersion, const BYTE lowVersion)
	{
		const auto versionRequired = MAKEWORD(highVersion, lowVersion);
		if(0 == WSAStartup(versionRequired, &_wsaData))
		{
			_available = _wsaData.wVersion == versionRequired;
		}
	}
	~WinsockStarter()
	{
		WSACleanup();
	}

	inline bool Available() const
	{
		return _available;
	}

private:
	bool _available = false;
	WSADATA _wsaData;
};