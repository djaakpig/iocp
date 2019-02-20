#include "TcpOperationRecv.h"
#include "Socket.h"
#include "TcpSession.h"

TcpOperationRecv::TcpOperationRecv( const DWORD capacity ) :
	_buf( capacity )
{
}

void TcpOperationRecv::Clear()
{
	_buf.Clear();
	__super::Clear();
}

void TcpOperationRecv::OnComplete( const int e )
{
	const auto r = _OnComplete( e );

	if( !r || !Post() )
	{
		_sessionPtr->Disconnect();
		Clear();
	}
}

bool TcpOperationRecv::Post()
{
	DWORD flags = 0;
	WSABUF wsaBuf{ 0, nullptr };
	const auto r = WSARecv( _sessionPtr->GetSocket()->GetValue(), &wsaBuf, 1, nullptr, &flags, this, nullptr );

	return SOCKET_ERROR != r ? true : _HandleError();
}

bool TcpOperationRecv::_OnComplete( const int e )
{
	if( e )
		return _Invoke( e, _sessionPtr, _buf );

	WSABUF wsaBuf;
	std::pair<int, DWORD> r{ ERROR_SUCCESS, 0 };

	while( _buf.BeginWrite( wsaBuf ) )
	{
		r = _Read( wsaBuf.buf, wsaBuf.len );

		if( 0 == r.second )
			break;

		_buf.EndWrite( r.second );

		if( r.first )
			break;
	}

	return _Invoke( r.first, _sessionPtr, _buf );
}

std::pair<int, DWORD> TcpOperationRecv::_Read( char* const pBuf, const int sz ) const
{
	const auto s = _sessionPtr->GetSocket()->GetValue();
	auto pCurrentBuf = pBuf;
	auto remainSize = sz;

	while( 0 < remainSize )
	{
		const auto r = ::recv( s, pCurrentBuf, remainSize, 0 );

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