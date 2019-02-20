#pragma once
#include <Windows.h>
#include <atomic>
#include <list>
#include <thread>

class IoOperation;

class IoService final
{
public:
	bool Associate( HANDLE h ) const;
	bool Post( IoOperation* const pOperation ) const;
	bool Start( const DWORD numWorkers );
	void Stop();

private:
	void _Run();

private:
	HANDLE _iocpHandle = nullptr;
	std::list<std::thread> _workers;
	std::atomic_size_t _numRunningWorkers;
};