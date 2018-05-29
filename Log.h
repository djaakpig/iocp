#pragma once
#include <iostream>

enum class ELogLevel : int
{
	normal = 0,
	warning = 1,
	error = 2,
	disable = 3,
};

ELogLevel GetLogLevel();
void SetLogLevel( const ELogLevel logLevel );

template<class T, class... Args>
inline void Log( T&& t, Args&&... args )
{
	cout << t;
	Log( forward<Args>( args )... );
}

template<class T>
inline void Log( T&& t )
{
	cout << t << endl;
}

template<class... Args>
inline void LogForce( Args&&... args )
{
	cout << "[FORCE] ";
	Log( forward<Args>( args )... );
}

template<class... Args>
inline void LogNormal( Args&&... args )
{
	if( GetLogLevel() <= ELogLevel::normal )
	{
		cout << "[NORMAL] ";
		Log( forward<Args>( args )... );
	}
}

template<class... Args>
inline void LogWarning( Args&&... args )
{
	if( GetLogLevel() <= ELogLevel::warning )
	{
		cout << "[WARNING] ";
		Log( forward<Args>( args )... );
	}
}

template<class... Args>
inline void LogError( Args&&... args )
{
	if( GetLogLevel() <= ELogLevel::error )
	{
		cout << "[ERROR] ";
		Log( forward<Args>( args )... );
	}
}