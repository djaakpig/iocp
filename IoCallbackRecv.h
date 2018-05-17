#pragma once
#include "IoCallback.h"

struct IoCallbackRecv : IoCallback
{
  using Fn = function<bool(const int, shared_ptr<TcpSession>, const DWORD)>;

  bool OnComplete(onst int e, cconst DWORD numBytes) override;
  void Reset() override;

  shared_ptr<TcpSession> sessionPtr;
  Fn fn;

private:
  char _buf[1024];
};