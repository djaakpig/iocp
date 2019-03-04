#pragma once
#include "Operation.h"
#include <memory>

class TcpOperation;

class TcpOperationError final : public Operation
{
private:
	int32_t _error = ERROR_SUCCESS;
	std::shared_ptr<TcpOperation> _operation{};

public:
	inline void SetError(const int32_t e)
	{
		_error = e;
	}
	inline void SetOperation(const std::shared_ptr<TcpOperation>& operation)
	{
		_operation = operation;
	}

	void Clear() override;
	void OnComplete(const int32_t) override;
};