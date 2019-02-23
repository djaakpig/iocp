#pragma once
#include <atomic>
#include "Operation.h"
#include "TcpOperationCallback.h"

class TcpOperation abstract : public Operation, public std::enable_shared_from_this<TcpOperation>
{
protected:
	std::shared_ptr<TcpSession> _session;

private:
	std::atomic_bool _inProgress = false;
	TcpOperationCallback _callback;

public:
	inline bool IsInProgress() const
	{
		return _inProgress;
	}
	inline void ResetInProgress()
	{
		_inProgress = false;
	}
	inline bool SetInProgress()
	{
		return _inProgress.exchange(true);
	}
	inline void SetCallback(TcpOperationCallback&& callback)
	{
		_callback = std::move(callback);
	}
	inline void SetSession(const std::shared_ptr<TcpSession>& session)
	{
		_session = session;
	}

	virtual void Clear();

protected:
	inline bool _Invoke(const int32_t e, const std::shared_ptr<TcpSession>& session) const
	{
		return _callback ? _callback(e, session) : false;
	}

	bool _HandleError();
};