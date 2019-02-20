#pragma once
#include <SockaddrIn.h>
#include <Type.h>

const WORD MinPort = 30000;
const WORD MaxPort = 30000;
const DWORD MaxSession = 1000;

class Arguments final
{
public:
	//	{{GET}}
	inline const SockaddrIn& GetAddr() const
	{
		return _addr;
	}
	inline ELogLevel GetLogLevel() const
	{
		return _logLevel;
	}
	inline DWORD GetNumSessions() const
	{
		return _numSessions;
	}
	inline DWORD GetNumWorkers() const
	{
		return _numWorkers;
	}
	inline const std::string& GetServiceName() const
	{
		return _serviceName;
	}
	inline bool IsServerMode() const
	{
		return _serverMode;
	}
	//	{{GET}}

	bool Load( const int argc, char** args );
	std::string ToTitle() const;

private:
	ELogLevel _logLevel = ELogLevel::normal;
	bool _serverMode = false;
	std::string _serviceName = "Unknown";
	DWORD _numWorkers = 4;
	std::string _ip = "0.0.0.0";
	WORD _port = 0;
	SockaddrIn _addr;
	DWORD _numSessions = 1;
};