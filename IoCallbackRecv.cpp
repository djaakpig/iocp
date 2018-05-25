#include "IoCallbackRecv.h"
#include "Socket.h"
#include "TcpSession.h"

IoCallbackRecv::IoCallbackRecv( const DWORD capacity ) : _buffer( capacity )
{
}

bool IoCallbackRecv::OnComplete( const int e )
{
	if( !_OnComplete( e ) )
		return false;

	return Post();
}

bool IoCallbackRecv::Post()
{
	DWORD flags = 0;
	WSABUF wsaBuf{ 0, nullptr };
	const auto r = WSARecv( _sessionPtr->GetSocket()->GetSocketHandle(), &wsaBuf, 1, nullptr, &flags, this, nullptr );

	if( SOCKET_ERROR == r )
	{
		const auto lastError = WSAGetLastError();
		if( WSA_IO_PENDING != lastError )
		{
			Clear();
			return false;
		}
	}

	SetInProgress();

	return true;
}

pair<int, DWORD> IoCallbackRecv::_Read( const SOCKET s, char* const pBuf, const int sz ) const
{
	auto pCurrentBuf = pBuf;
	auto remainSize = sz;

	while( 0 < remainSize )
	{
		const auto r = ::recv( s, pCurrentBuf, remainSize, 0 );

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

bool IoCallbackRecv::_OnComplete( const int e )
{
	if( e )
		return _Invoke( e, _sessionPtr, _buffer );

	const auto socket = _sessionPtr->GetSocket()->GetSocketHandle();
	WSABUF wsaBuf;
	pair<int, DWORD> r{ ERROR_SUCCESS, 0 };

	while( _buffer.BeginWrite( wsaBuf ) )
	{
		r = _Read( socket, wsaBuf.buf, wsaBuf.len );
		if( r.first || 0 == r.second )
			break;

		_buffer.EndWrite( r.second );
	}

	return _Invoke( r.first, _sessionPtr, _buffer );
}