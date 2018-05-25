#include "IoCallbackSend.h"
#include "Socket.h"
#include "TcpSession.h"

void IoCallbackSend::Enqueue( const WSABUF& buf )
{
	lock_guard<mutex> lock( _lock );

	_bufs.push_back( buf );
}

bool IoCallbackSend::OnComplete( const int e )
{
	if( e )
	{
		DoExclusive( _lock, [this, e]()
		{
			if( !_bufs.empty() )
			{
				_Invoke( e, _sessionPtr, _bufs.front() );
				_bufs.pop_front();

				for( const auto buf : _bufs )
					_Invoke( WSAECANCELLED, _sessionPtr, buf );
			}

			ResetInProgress();
		} );

		return false;
	}

	list<WSABUF> bufs;
	DoExclusive( _lock, bind( &list<WSABUF>::swap, ref( _bufs ), ref( bufs ) ) );

	while( !bufs.empty() )
	{
		const auto buf = bufs.front();
		const auto r = _Send( _sessionPtr->GetSocket()->GetSocketHandle(), buf.buf + _numSentBytes, buf.len - _numSentBytes );

		if( ERROR_SUCCESS != r.first )
		{
			bufs.pop_front();
			_numSentBytes = 0;

			_Invoke( r.first, _sessionPtr, buf );

			for( const auto cancelledBuf : bufs )
				_Invoke( WSAECANCELLED, _sessionPtr, cancelledBuf );

			break;
		}

		if( 0 == r.second )
			break;

		_numSentBytes += r.second;

		if( _numSentBytes < buf.len )
			break;

		bufs.pop_front();
		_numSentBytes = 0;

		_Invoke( ERROR_SUCCESS, _sessionPtr, buf );
	}

	const auto r = DoExclusive( _lock, [this, &bufs]()
	{
		_bufs.insert( _bufs.begin(), bufs.rbegin(), bufs.rend() );

		return _bufs.empty();
	} );

	if( !r ) return Post();

	ResetInProgress();

	return true;
}

bool IoCallbackSend::Post()
{
	_numSentBytes = 0;

	const DWORD flags = 0;
	WSABUF wsaBuf{ 0, nullptr };
	const auto r = WSASend( _sessionPtr->GetSocket()->GetSocketHandle(), &wsaBuf, 1, nullptr, flags, this, nullptr );

	if( SOCKET_ERROR == r )
	{
		const auto lastError = WSAGetLastError();
		if( WSA_IO_PENDING != lastError )
		{
			Clear();
			return false;
		}
	}

	return true;
}

pair<int, DWORD> IoCallbackSend::_Send( const SOCKET s, char* const pBuf, const int sz ) const
{
	auto pCurrentBuf = pBuf;
	auto remainSize = sz;

	while( 0 < remainSize )
	{
		const auto r = ::send( s, pCurrentBuf, remainSize, 0 );

		if( 0 == r )
			return{ WSAECONNRESET, 0 };

		if( SOCKET_ERROR == r )
		{
			const auto lastError = WSAGetLastError();
			if( WSAEWOULDBLOCK == lastError )
				break;

			return{ lastError, 0 };
		}

		pCurrentBuf += r;
		remainSize -= r;
	}

	return{ ERROR_SUCCESS, sz - remainSize };
}