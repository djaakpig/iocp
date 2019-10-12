#pragma once
#include "WsaBuf.h"

class CircularBuffer final
{
private:
	WsaBuf _buf;
	uint32_t _positionToWrite = 0;
	uint32_t _positionToRead = 0;
	uint32_t _size = 0;

public:
	explicit CircularBuffer(const uint32_t capacity);

	#pragma region getters
	inline bool IsNotEnough(const uint32_t needBytes) const
	{
		return _size < needBytes;
	}
	inline bool IsFull() const
	{
		return _buf->len <= _size;
	}
	inline auto GetSize() const->uint32_t
	{
		return _size;
	}
	#pragma endregion

	bool BeginRead(WSABUF& wsaBuf);
	bool BeginWrite(WSABUF& wsaBuf) const;
	void EndRead(const uint32_t numReadBytes);
	void EndWrite(const uint32_t numWrittenBytes);
	void Clear();

private:
	void _DoLinearize();
};