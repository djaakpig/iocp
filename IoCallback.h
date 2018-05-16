#pragma once
#include <windows.h>
#include <winsock2.h>
#include <function>

class IIoObject;
class IoCallback final : public OVERLAPPED
{
  using Fn=function<bool(IIoObject*, const DWORD)>;

public:
  IoCallback(const Fn&& fn) : _fn(fn)
  {
  }

  bool OnComplete(IIoObject* pObj, const DWORD numBytes) const
  {
    return _fn ? _fn(pObj, numBytes) : false;
  }

private:
  const Fn _fn;
};