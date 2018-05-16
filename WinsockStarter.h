#pragma once
#include <winsock.h>

class WinsockStarter final
{
public:
	explicit WinsockStarter(const WORD versionRequired)
	{
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