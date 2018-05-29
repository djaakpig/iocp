#pragma once
#include <Winsock2.h>
#include <algorithm>
#include "SockaddrIn.h"
#include "Log.h"
using namespace std;

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
	inline bool IsServerMode() const
	{
		return _serverMode;
	}
	//	{{GET}}

	bool Load( const int argc, char** args )
	{
		if( 7 != argc )
		{
			LogForce( args[0], " [LogLevel:0,1,2,3] [s or c] [numWorkers:1-4] [ip] [port:30000-40000] [numSessions:1-1000]" );
			return false;
		}

		_logLevel = max<ELogLevel>( min<ELogLevel>( ELogLevel::disable, static_cast<ELogLevel>(atoi( args[1] )) ), ELogLevel::normal );
		_serverMode = (0 == _stricmp( args[2], "s" ));
		_numWorkers = min<DWORD>( 4, max<DWORD>( 1, static_cast<DWORD>(atoi( args[3] )) ) );
		_ip = args[4];
		_port = min<WORD>( 40000, max<WORD>( 30000, static_cast<WORD>(atoi( args[5] )) ) );
		_numSessions = min<WORD>( 1000, max<WORD>( 1, static_cast<WORD>(atoi( args[6] )) ) );

		_addr.SetIP( _ip );
		_addr.SetPort( _port );

		return true;
	}

private:
	ELogLevel _logLevel = ELogLevel::normal;
	bool _serverMode = false;
	DWORD _numWorkers = 4;
	string _ip = "0.0.0.0";
	WORD _port = 0;
	SockaddrIn _addr;
	DWORD _numSessions = 1;
};