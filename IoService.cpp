#include "IoService.h"
#include "IoCallback.h"
#include "Socket.h"

bool IoService::Associate( const Socket* const pSocket ) const
{
	if( !_iocpHandle )
		return false;

	const auto r = CreateIoCompletionPort( pSocket->GetHandle(), _iocpHandle, NULL, 0 );
	if( !r )
	{
		const auto e = WSAGetLastError();
		return ERROR_INVALID_PARAMETER == e;
	}

	return _iocpHandle == r;
}

bool IoService::Post( LPOVERLAPPED pOverlapped ) const
{
	return TRUE == PostQueuedCompletionStatus( _iocpHandle, 0, NULL, pOverlapped );
}

bool IoService::Start( const DWORD numWorkers )
{
	_iocpHandle = CreateIoCompletionPort( INVALID_HANDLE_VALUE, nullptr, NULL, numWorkers );
	if( !_iocpHandle )
		return false;

	for( DWORD workerId = 0; numWorkers > workerId; ++workerId )
		_workers.push_back( thread( bind( &IoService::_Run, this ) ) );

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

	while( true )
	{
		const auto r = GetQueuedCompletionStatus( _iocpHandle, &numBytes, &cmpletionKey, &pOverlapped, INFINITE );

		if( !pOverlapped )
			break;

		const auto pCallback = static_cast<IoCallback*>(pOverlapped);
		const auto e = r ? ERROR_SUCCESS : WSAGetLastError();

		pCallback->OnComplete( e );
	}
}