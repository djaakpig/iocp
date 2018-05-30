#include "IoCallbackSend.h"
#include "Socket.h"
#include "TcpSession.h"
#include "WsaBuf.h"

void IoCallbackSend::Enqueue( const shared_ptr<WsaBuf>& buf )
{
	const unique_lock<mutex> l( _lock );
	_bufs.emplace_back( buf );
}

void IoCallbackSend::OnComplete( const int e )
{
	if( e )
	{
		DoExclusive( _lock, [this, e]()
		{
			_Invoke( e, _sessionPtr );

			_bufs.clear();
			_numSentBytes = 0;

			_sessionPtr->Disconnect();
			Clear();
		} );

		return;
	}

	auto numSentBytes = _numSentBytes;
	BufferPtrList bufs;
	DoExclusive( _lock, bind( &BufferPtrList::swap, ref( _bufs ), ref( bufs ) ) );

	while( !bufs.empty() )
	{
		const auto& buf = bufs.front()->Get();
		const auto r = _Send( _sessionPtr->GetSocket()->GetSocketHandle(), buf.buf + numSentBytes, buf.len - numSentBytes );

		numSentBytes += r.second;

		if( ERROR_SUCCESS != r.first )
		{
			_Invoke( r.first, _sessionPtr );

			break;
		}

		if( 0 == r.second )
			break;

		if( numSentBytes < buf.len )
			break;

		_Invoke( ERROR_SUCCESS, _sessionPtr );

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
			return{ WSAECONNRESET, sz - remainSize };

		if( SOCKET_ERROR == r )
		{
			const auto lastError = WSAGetLastError();
			if( WSAEWOULDBLOCK != lastError )
				return{ lastError, sz - remainSize };

			break;
		}

		pCurrentBuf += r;
		remainSize -= r;
	}

	return{ ERROR_SUCCESS, sz - remainSize };
}