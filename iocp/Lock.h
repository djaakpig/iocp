#pragma once
#include <shared_mutex>

template <class Fn, class... Args>
inline auto DoShared( std::shared_mutex& m, Fn&& fn, Args&&... args )
{
	const std::shared_lock<std::shared_mutex> s( m );
	return fn( args... );
}

template <class Mutex, class Fn, class... Args>
inline auto DoExclusive( Mutex& m, Fn&& fn, Args&&... args )
{
	const std::unique_lock<Mutex> s( m );
	return fn( args... );
}

template<class Fn, class... Args>
inline void WaitCondition( const std::chrono::milliseconds& sleepTime, Fn&& fn, Args&&... args )
{
	while( fn( args... ) )
		std::this_thread::sleep_for( sleepTime );
}