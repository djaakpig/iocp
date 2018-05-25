#include "IoService.h"
#include "IIoObject.h"
#include "IoCallback.h"

bool IoService::Associate( const IIoObject* const pObj )
{
	if( !_iocpHandle ) return false;

	const auto cmpl = reinterpret_cast<ULONG_PTR>(pObj);
	const auto r = CreateIoCompletionPort( pObj->GetHandle(), _iocpHandle, cmpl, 0 );
	if( !r )
	{
		const auto e = WSAGetLastError();
		return ERROR_INVALID_PARAMETER == e;
	}

	return _iocpHandle == r;
}

bool IoService::Start( const DWORD numWorkers )
{
	_iocpHandle = CreateIoCompletionPort( INVALID_HANDLE_VALUE, nullptr, NULL, numWorkers );
	if( !_iocpHandle ) return false;

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
	ULONG_PTR cmpl = 0;
	LPOVERLAPPED pOvl = nullptr;

	while( true )
	{
		const auto r = GetQueuedCompletionStatus( _iocpHandle, &numBytes, &cmpl, &pOvl, INFINITE );

		if( !pOvl )
			break;

		const auto pCallback = static_cast<IoCallback*>(pOvl);
		const auto e = r ? ERROR_SUCCESS : WSAGetLastError();

		if( !pCallback->OnComplete( e ) )
			pCallback->Clear();
	}
}