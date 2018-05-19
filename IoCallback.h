#pragma once
#include <WinSock2.h>
#include <memory>
#include <functional>
#include <atomic>
#include "NonCopyable.h"
using namespace std;

class TcpSession;
class IoCallback abstract : public OVERLAPPED, private NonCopyable
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
	inline void SetInProgress()
	{
		_inProgress = true;
	}
    //{{SET}}

	virtual void Clear();
	virtual bool OnComplete(const int e, const DWORD numBytes) = 0;
	void Reset();

protected:
	shared_ptr<TcpSession> _sessionPtr;

private:
	atomic_bool _inProgress = false;
};