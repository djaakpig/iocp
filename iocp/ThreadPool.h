#pragma once
#include <Windows.h>
#include <atomic>
#include <list>
#include <thread>

class Operation;

class ThreadPool final
{
private:
	HANDLE _iocpHandle = nullptr;
	std::list<std::thread> _workers{};
	std::atomic_size_t _numRunningWorkers{0};

public:
	ThreadPool() = default;
	~ThreadPool() = default;

	bool Associate(HANDLE h) const;
	bool Post(Operation* const pOperation) const;
	bool Start(const uint32_t numWorkers);
	void Stop();

private:
	void _Run();
};