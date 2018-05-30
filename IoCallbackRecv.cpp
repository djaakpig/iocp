#include "IoCallbackRecv.h"
#include "Socket.h"
#include "TcpSession.h"

IoCallbackRecv::IoCallbackRecv( const DWORD capacity ) :
	_buffer( capacity )
{
}

void IoCallbackRecv::OnComplete( const int e )
{
	const auto r = _OnComplete( e );

	if( r )
	{
		Post();
	}
	else
	{
		_sessionPtr->Disconnect();
		Clear();
	}
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
			if( !_sessionPtr->PostError( lastError, shared_from_this() ) )
				return false;
		}
	}

	return true;
}

pair<int, DWORD> IoCallbackRecv::_Read( char* const pBuf, const int sz ) const
{
	auto pCurrentBuf = pBuf;
	auto remainSize = sz;

	while( 0 < remainSize )
	{
		const auto r = ::recv( _sessionPtr->GetSocket()->GetSocketHandle(), pCurrentBuf, remainSize, 0 );

		if( 0 == r )
			return{ WSAECONNRESET, sz - remainSize };

		if( SOCKET_ERROR == r )
		{
			const auto lastError = WSAGetLastError();
			if( WSAEWOULDBLOCK == lastError )
				break;

			return{ lastError, sz - remainSize };
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

	WSABUF wsaBuf;
	pair<int, DWORD> r{ ERROR_SUCCESS, 0 };

	while( _buffer.BeginWrite( wsaBuf ) )
	{
		r = _Read( wsaBuf.buf, wsaBuf.len );

		if( 0 == r.second )
			break;

		_buffer.EndWrite( r.second );

		if( r.first )
			break;
	}

	return _Invoke( r.first, _sessionPtr, _buffer );
}