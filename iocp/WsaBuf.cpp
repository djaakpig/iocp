#include "WsaBuf.h"
#include <algorithm>
#include "Type.h"

WsaBuf::WsaBuf()
{
	_buf = { 0, nullptr };
}

WsaBuf::WsaBuf( const DWORD len )
{
	_buf.buf = reinterpret_cast<char*>( malloc( len ) );
	_buf.len = len;
}

WsaBuf::WsaBuf( const WSABUF& buf )
{
	_Clone( buf.buf, buf.len );
}

WsaBuf::WsaBuf( const char* const pBuf, const DWORD len )
{
	_Clone( pBuf, len );
}

WsaBuf::~WsaBuf()
{
	if( _buf.buf )
		free( _buf.buf );

	_buf = { 0, nullptr };
}

void WsaBuf::CopyFrom( const char* const pBuf, const DWORD len )
{
	if( _buf.len < len )
	{
		if( !_buf.buf )
			_buf.buf = reinterpret_cast<char*>( malloc( len ) );
		else
			_buf.buf = reinterpret_cast<char*>( realloc( _buf.buf, len ) );

		_buf.len = len;
	}

	memcpy( _buf.buf, pBuf, len );
}

void WsaBuf::Move( const DWORD srcOffset, const DWORD dstOffset, const DWORD len )
{
	if( _buf.buf )
		memmove( _buf.buf + dstOffset, _buf.buf + srcOffset, std::min<DWORD>( len, _buf.len - dstOffset ) );
}

void WsaBuf::_Clone( const char* const pBuf, const DWORD len )
{
	_buf.buf = reinterpret_cast<char*>( malloc( len + sizeof( PacketLength ) ) );
	_buf.len = len + sizeof( PacketLength );
	memcpy( _buf.buf + sizeof( PacketLength ), pBuf, len );

	*reinterpret_cast<PacketLength*>( _buf.buf ) = _buf.len;
}