#include "Arguments.h"
#include <algorithm>
#include <sstream>
#include <thread>
#include <Log.h>

bool Arguments::Load(const int32_t argc, char** args)
{
	if(7 != argc)
	{
		LogForce(args[0], " [LogLevel:0,1,2,3] [s or c] [numWorkers:1-4] [ip] [port:30000-40000] [numSessions:1-1000]");
		return false;
	}

	const auto rawLogLevel = static_cast<ELogLevel>(atoi(args[1]));
	const auto rawNumWorkers = static_cast<uint32_t>(atoi(args[3]));
	const auto rawPort = static_cast<uint16_t>(atoi(args[5]));
	const auto rawNumSessions = static_cast<uint32_t>(atoi(args[6]));

	_logLevel = std::max<ELogLevel>(std::min<ELogLevel>(ELogLevel::disable, rawLogLevel), ELogLevel::normal);
	_serverMode = (0 == _stricmp(args[2], "s"));
	_serviceName = _serverMode ? "SERVER" : "CLIENT";
	_numWorkers = std::min<uint32_t>(std::thread::hardware_concurrency() * 2, std::max<uint32_t>(1, rawNumWorkers));
	_ip = args[4];
	_port = std::min<uint16_t>(MaxPort, std::max<uint16_t>(MinPort, rawPort));
	_numSessions = std::min<uint32_t>(MaxSession, std::max<uint32_t>(1, rawNumSessions));

	_addr.SetIP(_ip);
	_addr.SetPort(_port);

	return true;
}

std::string Arguments::ToTitle() const
{
	std::stringstream ss;
	ss << "[" << _serviceName << "] IP:" << _ip << ", PORT:" << _port << ", NumSessions:" << _numSessions;
	return ss.str();
}