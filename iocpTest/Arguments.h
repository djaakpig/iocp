#pragma once
#include <Type.h>
#include <SockaddrIn.h>

const uint16_t MinPort = 30000;
const uint16_t MaxPort = 30000;
const uint32_t MaxSession = 1000;

class Arguments final
{
private:
	ELogLevel _logLevel = ELogLevel::normal;
	bool _serverMode = false;
	std::string _serviceName = "Unknown";
	uint32_t _numWorkers = 4;
	std::string _ip = "0.0.0.0";
	WORD _port = 0;
	SockaddrIn _addr;
	uint32_t _numSessions = 1;

public:
	inline auto GetAddr() const->const SockaddrIn&
	{
		return _addr;
	}
	inline auto GetLogLevel() const->ELogLevel
	{
		return _logLevel;
	}
	inline auto GetNumSessions() const->uint32_t
	{
		return _numSessions;
	}
	inline auto GetNumWorkers() const->uint32_t
	{
		return _numWorkers;
	}
	inline auto GetServiceName() const->const std::string&
	{
		return _serviceName;
	}
	inline bool IsServerMode() const
	{
		return _serverMode;
	}

	bool Load(const int32_t argc, char** args);
	auto ToTitle() const->std::string;
};