#include "IoService.h"
#include "Lock.h"
#include "Log.h"
#include "IoCallback.h"

bool IoService::Associate( HANDLE h ) const
{
	if( !_iocpHandle )
		return false;

	const auto r = CreateIoCompletionPort( h, _iocpHandle, NULL/*reinterpret_cast<ULONG_PTR>(h)*/, 0 );
	if( !r )
	{
		const auto e = WSAGetLastError();
		return ERROR_INVALID_PARAMETER == e;
	}

	return _iocpHandle == r;
}

bool IoService::Post( IoCallback* const pCallback ) const
{
	if( !pCallback )
		return false;

	return TRUE == PostQueuedCompletionStatus( _iocpHandle, 0, NULL, pCallback );
}

bool IoService::Start( const DWORD numWorkers )
{
	_iocpHandle = CreateIoCompletionPort( INVALID_HANDLE_VALUE, nullptr, NULL, numWorkers );
	if( !_iocpHandle )
		return false;

	for( DWORD workerId = 0; numWorkers > workerId; ++workerId )
		_workers.emplace_back( thread( bind( &IoService::_Run, this ) ) );

	WaitCondition( chrono::milliseconds( 100 ), [this]
	{
		return _numRunningWorkers < _workers.size();
	} );

	return true;
}

void IoService::Stop()
{
	for( size_t workerId = 0; _workers.size() > workerId; ++workerId )
		PostQueuedCompletionStatus( _iocpHandle, 0, NULL, nullptr );

	for( auto& w : _workers )
	{
		if( w.joinable() )
			w.join();
	}
	_workers.clear();

	if( _iocpHandle )
	{
		CloseHandle( _iocpHandle );
		_iocpHandle = nullptr;
	}
}

void IoService::_Run()
{
	DWORD numBytes = 0;
	ULONG_PTR cmpletionKey = NULL;
	LPOVERLAPPED pOverlapped = nullptr;

	++_numRunningWorkers;

	while( true )
	{
		const auto r = GetQueuedCompletionStatus( _iocpHandle, &numBytes, &cmpletionKey, &pOverlapped, INFINITE );

		if( !pOverlapped )
			break;

		const auto pCallback = static_cast<IoCallback*>(pOverlapped);
		const auto e = r ? ERROR_SUCCESS : WSAGetLastError();

		pCallback->OnComplete( e );
	}

	--_numRunningWorkers;
}