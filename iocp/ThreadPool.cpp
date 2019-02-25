#include "ThreadPool.h"
#include "Lock.h"
#include "Log.h"
#include "Operation.h"

bool ThreadPool::Associate(HANDLE h) const
{
	if(!_iocpHandle)
		return false;

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
		return false;

	return TRUE == PostQueuedCompletionStatus(_iocpHandle, 0, NULL, pOperation);
}

bool ThreadPool::Start(const uint32_t numWorkers)
{
	_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, numWorkers);
	if(!_iocpHandle)
		return false;

	for(uint32_t workerId = 0; numWorkers > workerId; ++workerId)
		_workers.emplace_back(std::thread(std::bind(&ThreadPool::_Run, this)));

	WaitCondition(std::chrono::milliseconds(100), [this]
	{
		return _numRunningWorkers < _workers.size();
	});

	return true;
}

void ThreadPool::Stop()
{
	for(size_t workerId = 0; _workers.size() > workerId; ++workerId)
		PostQueuedCompletionStatus(_iocpHandle, 0, NULL, nullptr);

	for(auto& w : _workers)
	{
		if(w.joinable())
			w.join();
	}
	_workers.clear();

	if(_iocpHandle)
	{
		CloseHandle(_iocpHandle);
		_iocpHandle = nullptr;
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

		if(!pOverlapped)
			break;

		const auto pCallback = static_cast<Operation*>(pOverlapped);
		const auto e = r ? ERROR_SUCCESS : WSAGetLastError();

		pCallback->OnComplete(e);
	}

	--_numRunningWorkers;
}