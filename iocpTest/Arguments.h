#pragma once
#include <SockaddrIn.h>
#include <Type.h>
using namespace std;

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
	inline const string& GetServiceName() const
	{
		return _serviceName;
	}
	inline bool IsServerMode() const
	{
		return _serverMode;
	}
	//	{{GET}}

	bool Load( const int argc, char** args );
	string ToTitle() const;

private:
	ELogLevel _logLevel = ELogLevel::normal;
	bool _serverMode = false;
	string _serviceName = "Unknown";
	DWORD _numWorkers = 4;
	string _ip = "0.0.0.0";
	WORD _port = 0;
	SockaddrIn _addr;
	DWORD _numSessions = 1;
};