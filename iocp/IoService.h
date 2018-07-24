#pragma once
#include <Windows.h>
#include <atomic>
#include <list>
#include <thread>
using namespace std;

class IoCallback;

class IoService final
{
public:
	bool Associate( HANDLE h ) const;
	bool Post( IoCallback* const pCallback ) const;
	bool Start( const DWORD numWorkers );
	void Stop();

private:
	void _Run();

private:
	HANDLE _iocpHandle = nullptr;
	list<thread> _workers;
	atomic_size_t _numRunningWorkers;
};