#pragma once
#include "IoCallback.h"

struct IoCallbackSend : IoCallback
{
  using Fn = function<bool(const int, shared_ptr<TcpSession>, const DWORD)>;

  bool OnComplete(const DWORD numBytes) override;
  void Reset() override;

  Fn fn;

private:
  char _buf[1024];
};