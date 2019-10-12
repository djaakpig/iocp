#pragma once
#include "TcpOperation.h"
#include "Lock.h"
#include <list>

class WsaBuf;

class TcpOperationSend final : public TcpOperation
{
	using BufferPtrList = std::list<std::shared_ptr<WsaBuf>>;

private:
	std::mutex _lock;
	BufferPtrList _bufs;
	uint32_t _numSentBytes = 0;

public:
	void Enqueue(const std::shared_ptr<WsaBuf>& buf);
	void OnComplete(const int32_t e) override;
	bool Post();

private:
	bool _OnComplete(const int32_t e);
	auto _Send(char* const pBuf, const int32_t sz) const->std::pair<int32_t, uint32_t>;
};