#include "ThreadPool.h"
#include "Lock.h"
#include "Log.h"
#include "Operation.h"
using namespace std::chrono;

bool ThreadPool::Associate(HANDLE h) const
{
	if(INVALID_HANDLE_VALUE == _iocpHandle)
	{
		return false;
	}

	const auto r = CreateIoCompletionPort(h, _iocpHandle, NULL/*reinterpret_cast<ULONG_PTR>(h)*/, 0);

	if(!r)
	{
		const auto e = WSAGetLastError();

		return ERROR_INVALID_PARAMETER == e;
	}

	return _iocpHandle == r;
}

bool ThreadPool::Post(Operation* const pOperation) const
{
	if(nullptr == pOperation)
	{
		return false;
	}

	return TRUE == PostQueuedCompletionStatus(_iocpHandle, 0, NULL, pOperation);
}

bool ThreadPool::Start(const uint32_t numWorkers)
{
	_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, numWorkers);

	if(INVALID_HANDLE_VALUE == _iocpHandle)
	{
		return false;
	}

	for(uint32_t workerId = 0; numWorkers > workerId; ++workerId)
	{
		_workers.emplace_back(std::thread{[this]
		{
			this->_Run();
		}});
	}

	WaitCondition(100ms, [this]
	{
		return _numRunningWorkers < _workers.size();
	});

	return true;
}

void ThreadPool::Stop()
{
	for(size_t workerId = 0; _workers.size() > workerId; ++workerId)
	{
		PostQueuedCompletionStatus(_iocpHandle, 0, NULL, nullptr);
	}

	for(auto& w : _workers)
	{
		if(w.joinable())
		{
			w.join();
		}
	}
	_workers.clear();

	if(INVALID_HANDLE_VALUE != _iocpHandle)
	{
		CloseHandle(_iocpHandle);
		_iocpHandle = INVALID_HANDLE_VALUE;
	}
}

void ThreadPool::_Run()
{
	DWORD numBytes = 0;
	ULONG_PTR completionKey = NULL;
	LPOVERLAPPED pOverlapped = nullptr;

	++_numRunningWorkers;

	while(true)
	{
		const auto r = GetQueuedCompletionStatus(_iocpHandle, &numBytes, &completionKey, &pOverlapped, INFINITE);

		if(nullptr == pOverlapped)
		{
			break;
		}

		const auto pCallback = static_cast<Operation*>(pOverlapped);
		const auto e = r ? ERROR_SUCCESS : WSAGetLastError();

		++_numRunningCallbacks;
		pCallback->OnComplete(e);
		--_numRunningCallbacks;
	}

	--_numRunningWorkers;
}