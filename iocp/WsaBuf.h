#pragma once
#include <WinSock2.h>
#include <memory>

class WsaBuf final
{
private:
	WSABUF _buf;

public:
	WsaBuf();
	explicit WsaBuf(const uint32_t len);
	explicit WsaBuf(const WSABUF& buf);
	WsaBuf(const char* const pBuf, const uint32_t len);
	~WsaBuf();

	#pragma region getters
	inline auto Get() const->const WSABUF&
	{
		return _buf;
	}
	inline auto operator ->()->WSABUF*
	{
		return &_buf;
	}
	inline auto operator ->() const->const WSABUF*
	{
		return &_buf;
	}
	#pragma endregion

	void CopyFrom(const char* const pBuf, const uint32_t len);
	void Move(const uint32_t srcOffset, const uint32_t dstOffset, const uint32_t len);
};