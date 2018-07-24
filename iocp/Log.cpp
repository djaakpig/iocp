#include "Log.h"
#include "Lock.h"

//#define LOG_SAFETY

ELogLevel LogLevel = ELogLevel::normal;
#ifdef LOG_SAFETY
mutex LogLock;
#endif

void BeginLogLock()
{
	#ifdef LOG_SAFETY
	LogLock.lock();
	#endif
}

void EndLogLock()
{
	#ifdef LOG_SAFETY
	LogLock.unlock();
	#endif
}

ELogLevel GetLogLevel()
{
	return LogLevel;
}

void SetLogLevel( const ELogLevel logLevel )
{
	LogLevel = logLevel;
}