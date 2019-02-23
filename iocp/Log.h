#pragma once
#include <iostream>
#include "Type.h"

void BeginLogLock();
void EndLogLock();
ELogLevel GetLogLevel();
void SetLogLevel(const ELogLevel logLevel);

template<class T, class... Args>
inline void Log(T&& t, Args&&... args)
{
	std::cout << t;
	Log(std::forward<Args>(args)...);
}

template<class T>
inline void Log(T&& t)
{
	std::cout << t << std::endl;
}

template<class... Args>
inline void LogSafety(Args&&... args)
{
	BeginLogLock();
	Log(std::forward<Args>(args)...);
	EndLogLock();
}

template<class... Args>
inline void LogForce(Args&&... args)
{
	LogSafety("[FORCE] ", std::forward<Args>(args)...);
}

template<class... Args>
inline void LogNormal(Args&&... args)
{
	if(GetLogLevel() <= ELogLevel::normal)
		LogSafety("[NORMAL] ", std::forward<Args>(args)...);
}

template<class... Args>
inline void LogWarning(Args&&... args)
{
	if(GetLogLevel() <= ELogLevel::warning)
		LogSafety("[WARNING] ", std::forward<Args>(args)...);
}

template<class... Args>
inline void LogError(Args&&... args)
{
	if(GetLogLevel() <= ELogLevel::error)
		LogSafety("[ERROR] ", std::forward<Args>(args)...);
}