#include "Log.h"

ELogLevel LogLevel = ELogLevel::normal;

ELogLevel GetLogLevel()
{
	return LogLevel;
}

void SetLogLevel( const ELogLevel logLevel )
{
	LogLevel = logLevel;
}