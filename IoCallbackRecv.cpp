#include "IoCallbackRecv.h"
#include "TcpSession.h"
#include "Socket.h"
#include <system_error>

IoCallbackRecv::IoCallbackRecv( const DWORD capacity ) : _buffer( capacity )
{
}

bool IoCallbackRecv::OnComplete( const int e, const DWORD numBytes )
{
	if( e ) return _fn( e, _sessionPtr,_buffer  );

	const auto socket = _sessionPtr->GetSocket()->GetSocketHandle();
	WSABUF wsaBuf;
	DWORD numReadBytes = 0;
	DWORD numWrittenBytes = 0;

	try
	{
		while( _buffer.BeginWrite( wsaBuf ) )
		{
			numReadBytes = _Read( socket, wsaBuf.buf, wsaBuf.len );
			if( 0 == numReadBytes )
				break;

			_buffer.EndWrite( numReadBytes );

			numWrittenBytes += numReadBytes;
		}

		return _fn( ERROR_SUCCESS, _sessionPtr, _buffer );
	}
	catch( const system_error& e)
	{
		_fn( e.code().value(), _sessionPtr,_buffer  );
		return false;
	}
}

DWORD IoCallbackRecv::_Read( const SOCKET s, char* const pBuf, const int sz ) const
{
	auto pCurrentBuf = pBuf;
	auto remainSize = sz;

	while( 0 < remainSize )
	{
		const auto r = ::recv( s, pCurrentBuf, remainSize, 0 );

		if( 0 == r )
			throw system_error(error_code( WSAECONNRESET, system_category() ));

		if( SOCKET_ERROR == r )
		{
			const auto lastError = WSAGetLastError();
			if( WSAEWOULDBLOCK == lastError )
				break;

			throw system_error( error_code( lastError, system_category() ) );
		}

		pCurrentBuf += r;
		remainSize -= r;
	}

	return sz - remainSize;
}