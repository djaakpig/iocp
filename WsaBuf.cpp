#include "WsaBuf.h"
#include "Type.h"

WsaBuf::WsaBuf()
{
	_buf = { 0, nullptr };
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

void WsaBuf::_Clone( const char* const pBuf, const DWORD len )
{
	_buf.buf = reinterpret_cast<char*>( malloc( len + sizeof( PacketLength ) ) );
	_buf.len = len + sizeof( PacketLength );
	memcpy( _buf.buf + sizeof( PacketLength ), pBuf, len );

	*reinterpret_cast<PacketLength*>( _buf.buf ) = _buf.len;
}