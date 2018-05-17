#pragma once
#include <Windows.h>
#include <list>
#include <thread>
using namespace std;

__interface IIoObject;
class IoService final
{
public:
	bool Associate(const IIoObject* const pObj);
	bool Start(const DWORD numWorkers);
	void Stop();

private:
	void _Run();

private:
	HANDLE _iocpHandle = nullptr;
	list<thread> _workers;
};