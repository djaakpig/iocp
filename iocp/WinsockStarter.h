#pragma once
#include <WinSock2.h>

class WinsockStarter final
{
private:
	bool _available = false;
	WSADATA _wsaData;

public:
	WinsockStarter()
	{
		const auto versionRequired = MAKEWORD(2, 2);

		if(0 == ::WSAStartup(versionRequired, &_wsaData))
		{
			_available = _wsaData.wVersion == versionRequired;
		}
	}
	~WinsockStarter()
	{
		::WSACleanup();
	}

	#pragma region getters
	inline bool Available() const
	{
		return _available;
	}
	#pragma endregion
};