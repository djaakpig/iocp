#pragma once
#include "IoOperation.h"
#include <atomic>
#include <memory>

class TcpSession;

class TcpOperation abstract : public IoOperation, public std::enable_shared_from_this<TcpOperation>
{
public:
	//	{{GET}}
	inline bool IsInProgress() const
	{
		return _inProgress;
	}
	//	{{GET}}

	//	{{SET}}
	inline void ResetInProgress()
	{
		_inProgress = false;
	}
	inline bool SetInProgress()
	{
		return _inProgress.exchange( true );
	}
	inline void SetSession( const std::shared_ptr<TcpSession>& sessionPtr )
	{
		_sessionPtr = sessionPtr;
	}
	//	{{SET}}

	virtual void Clear();

protected:
	bool _HandleError();

protected:
	std::shared_ptr<TcpSession> _sessionPtr;

private:
	std::atomic_bool _inProgress = false;
};