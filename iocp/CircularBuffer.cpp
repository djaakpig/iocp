#include "CircularBuffer.h"
#include "Global.h"

thread_local WsaBuf RecvBufForLinearize;

CircularBuffer::CircularBuffer(const uint32_t capacity) :
	_buf(capacity)
{
}

bool CircularBuffer::BeginRead(WSABUF& wsaBuf)
{
	if(0 == wsaBuf.len)
		return false;

	if(IsNotEnough(wsaBuf.len))
		return false;

	const auto bufLimit = _positionToRead > _positionToWrite ? _buf->len : _positionToWrite;

	if(bufLimit - _positionToRead < wsaBuf.len)
		_DoLinearize();

	wsaBuf.buf = _buf->buf + _positionToRead;

	return true;
}

bool CircularBuffer::BeginWrite(WSABUF& wsaBuf) const
{
	if(IsFull())
		return false;

	const auto bufLimit = _positionToRead > _positionToWrite ? _positionToRead : _buf->len;
	wsaBuf = {bufLimit - _positionToWrite, _buf->buf + _positionToWrite};

	return true;
}

void CircularBuffer::EndRead(const uint32_t numReadBytes)
{
	_positionToRead = (_positionToRead + numReadBytes) % _buf->len;
	_size -= numReadBytes;
}

void CircularBuffer::EndWrite(const uint32_t numWrittenBytes)
{
	_positionToWrite = (_positionToWrite + numWrittenBytes) % _buf->len;
	_size += numWrittenBytes;
}

void CircularBuffer::Clear()
{
	_positionToWrite = 0;
	_positionToRead = 0;
	_size = 0;
}

void CircularBuffer::_DoLinearize()
{
	const auto readSize = _buf->len - _positionToRead;

	RecvBufForLinearize.CopyFrom(_buf->buf + _positionToRead, readSize);
	_buf.Move(0, readSize, _positionToWrite);
	_buf.CopyFrom(RecvBufForLinearize->buf, readSize);

	_positionToRead = 0;
	_positionToWrite = _size;
}