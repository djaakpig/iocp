#pragma once
#include "IoCallback.h"

struct IoCallbackReuse : IoCallback
{
  using Fn = function<bool(const int, shared_ptr<TcpSession>)>;

  bool OnComplete(onst int e, cconst DWORD numBytes) override;
  void Reset() override;

  shared_ptr<TcpSession> sessionPtr;
  Fn fn;
};