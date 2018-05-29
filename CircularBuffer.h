#pragma once
#include <WinSock2.h>

class CircularBuffer final
{
public:
	explicit CircularBuffer(const DWORD capacity);
	~CircularBuffer();

	//	{{GET}}
	inline bool IsEmpty() const
	{
		return 0 == _size;
	}
	inline bool IsFull() const
	{
		return _capacity <= _size;
	}
	inline DWORD GetSize() const
	{
		return _size;
	}
	//	{{GET}}

	bool BeginRead(WSABUF& wsaBuf) const;
	bool BeginWrite(WSABUF& wsaBuf) const;
	void EndRead(const DWORD numReadBytes);
	void EndWrite(const DWORD numWrittenBytes);
	void Clear();

private:
	char* _pBuf = nullptr;
	DWORD _capacity = 0;
	DWORD _positionToWrite = 0;
	DWORD _positionToRead = 0;
	DWORD _size = 0;
};