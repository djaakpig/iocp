#pragma once
#include "TcpOperation.h"
#include "CircularBuffer.h"

class TcpOperationRecv final : public TcpOperation
{
private:
	CircularBuffer _buf;

public:
	explicit TcpOperationRecv(const uint32_t capacity);

	bool BeginRead(WSABUF& wsaBuf);
	void Clear() override;
	void EndRead(const uint32_t numReadBytes);
	void OnComplete(const int32_t e) override;
	bool Post();

private:
	bool _OnComplete(const int32_t e);
	auto _Read(WSABUF& wsaBuf) const->std::pair<int32_t, uint32_t>;
};