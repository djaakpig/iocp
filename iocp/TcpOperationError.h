#pragma once
#include "IoOperation.h"
#include <memory>
using namespace std;

class TcpOperation;

class TcpOperationError final : public IoOperation
{
public:
	//	{{SET}}
	inline void SetError( const int e )
	{
		_error = e;
	}
	inline void SetOperation( const shared_ptr<TcpOperation>& operationPtr )
	{
		_operationPtr = operationPtr;
	}
	//	{{SET}}

	void Clear() override;
	void OnComplete( const int ) override;

private:
	int _error = ERROR_SUCCESS;
	shared_ptr<TcpOperation> _operationPtr;
};