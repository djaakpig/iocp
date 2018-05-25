#pragma once
#include <WinSock2.h>
#include <atomic>
#include <memory>
using namespace std;

class TcpSession;
class IoCallback abstract : public OVERLAPPED
{
public:
	IoCallback();
	virtual ~IoCallback() = default;

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
	inline void SetSession( const shared_ptr<TcpSession>& sessionPtr )
	{
		_sessionPtr = sessionPtr;
	}
	//	{{SET}}

	virtual void Clear();
	virtual bool OnComplete(const int) = 0;
	void Reset();

protected:
	shared_ptr<TcpSession> _sessionPtr;

private:
	atomic_bool _inProgress = false;
};