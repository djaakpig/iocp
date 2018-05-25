#pragma once
#include <shared_mutex>
using namespace std;

template <class Fn, class... Args>
inline auto DoShared( shared_mutex& m, Fn&& fn, Args&&... args )
{
	const shared_lock<shared_mutex> s( m );
	return fn( args... );
}

template <class Mutex, class Fn, class... Args>
inline auto DoExclusive( Mutex& m, Fn&& fn, Args&&... args )
{
	const unique_lock<Mutex> s( m );
	return fn( args... );
}

template<class Fn, class... Args>
inline void WaitCondition( const int sleepTime, Fn&& fn, Args&&... args )
{
	while( fn( args... ) )
		this_thread::sleep_for( chrono::milliseconds( sleepTime ) );
}