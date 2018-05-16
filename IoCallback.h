#pragma once
#include <Windows.h>
#include <WinSock2.h>
#include <functional>
using namespace std;

class IIoObject;
class IoCallback final : public OVERLAPPED
{
public:
	using Fn = function<bool(const int, IIoObject* const, const DWORD)>;

public:
	IoCallback() = default;
	explicit IoCallback(const Fn&& fn) : _fn(fn)
	{
	}

	inline operator =(const Fn&& fn)
	{
		_fn = fn;
	}

	inline bool Invoke(const int e, IIoObject* const pObj, const DWORD numBytes) const
	{
		return _fn ? _fn(e, pObj, numBytes) : false;
	}

private:
	Fn _fn = nullptr;
};