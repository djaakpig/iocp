#include "CircularBuffer.h"
#include "Global.h"

CircularBuffer::CircularBuffer( const DWORD capacity ) : _capacity( capacity )
{
	_pBuf = new char[capacity];
}

CircularBuffer::~CircularBuffer()
{
	SafeDeleteArray( _pBuf );
}

bool CircularBuffer::BeginRead( WSABUF& wsaBuf ) const
{
	if( IsEmpty() ) return false;

	const auto bufLimit = _positionToRead > _positionToWrite ? _capacity : _positionToWrite;
	wsaBuf = { bufLimit - _positionToRead, _pBuf + _positionToRead };

	return true;
}

bool CircularBuffer::BeginWrite( WSABUF& wsaBuf ) const
{
	if( IsFull() ) return false;

	const auto bufLimit = _positionToRead > _positionToWrite ? _positionToRead : _capacity;
	wsaBuf = { bufLimit - _positionToWrite, _pBuf + _positionToWrite };

	return true;
}

void CircularBuffer::EndRead( const DWORD numReadBytes )
{
	_positionToRead = (_positionToRead + numReadBytes) % _capacity;
	_size += numReadBytes;
}

void CircularBuffer::EndWrite( const DWORD numWrittenBytes )
{
	_positionToWrite = (_positionToWrite + numWrittenBytes) % _capacity;
	_size += numWrittenBytes;
}

void CircularBuffer::Reset()
{
	_size = 0;
	_positionToWrite = 0;
	_positionToRead = 0;
}