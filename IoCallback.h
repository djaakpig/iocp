#pragma once
#include <WinSock2.h>
#include <atomic>
using namespace std;

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
	inline void SetInProgress()
	{
		_inProgress = true;
	}
	//	{{SET}}

	virtual void Clear() = 0;
	virtual bool OnComplete() = 0;
	virtual bool Post() = 0;
	void Reset();

private:
	atomic_bool _inProgress = false;
};