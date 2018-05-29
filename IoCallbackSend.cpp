#include "IoCallbackSend.h"
#include "Socket.h"
#include "TcpSession.h"

void IoCallbackSend::Enqueue( const WSABUF& buf )
{
	lock_guard<mutex> lock( _lock );

	_bufs.push_back( buf );
}

void IoCallbackSend::OnComplete( const int e )
{
	if( e )
	{
		DoExclusive( _lock, [this, e]()
		{
			auto iter = _bufs.begin();

			_Invoke( e, _sessionPtr, *iter );

			while( _bufs.end() != ++iter )
				_Invoke( WSAECANCELLED, _sessionPtr, *iter );

			_bufs.clear();
			_numSentBytes = 0;

			_sessionPtr->Disconnect();
			Clear();
		} );

		return;
	}

	auto numSentBytes = _numSentBytes;
	list<WSABUF> bufs;
	DoExclusive( _lock, bind( &list<WSABUF>::swap, ref( _bufs ), ref( bufs ) ) );

	while( !bufs.empty() )
	{
		const auto buf = bufs.front();
		const auto r = _Send( _sessionPtr->GetSocket()->GetSocketHandle(), buf.buf + numSentBytes, buf.len - numSentBytes );

		if( ERROR_SUCCESS != r.first )
		{
			_Invoke( r.first, _sessionPtr, buf );

			bufs.pop_front();
			numSentBytes = 0;

			break;
		}

		if( 0 == r.second )
			break;

		numSentBytes += r.second;

		if( numSentBytes < buf.len )
			break;

		_Invoke( ERROR_SUCCESS, _sessionPtr, buf );

		bufs.pop_front();
		numSentBytes = 0;
	}

	_numSentBytes = numSentBytes;

	DoExclusive( _lock, [this, &bufs]()
	{
		_bufs.insert( _bufs.begin(), bufs.rbegin(), bufs.rend() );

		if( _bufs.empty() )
			Clear();
		else
			Post();
	} );
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
			if( !_sessionPtr->PostError( lastError, shared_from_this() ) )
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