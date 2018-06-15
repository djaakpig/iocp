#pragma once
#include <Windows.h>
#include <list>
#include <thread>
using namespace std;

class IoService final
{
public:
	bool Associate( HANDLE h ) const;
	bool Post( LPOVERLAPPED pOverlapped ) const;
	bool Start( const DWORD numWorkers );
	void Stop();

private:
	void _Run();

private:
	HANDLE _iocpHandle = nullptr;
	list<thread> _workers;
};