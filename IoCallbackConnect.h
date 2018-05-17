#pragma once
#include "IoCallback.h"

class TcpSession;
struct IoCallbackConnect : IoCallback
{
  using Fn = function<bool(const int, shared_ptr<TcpSession>)>;

  bool OnComplete(const int e, const DWORD numBytes) override;
  void Reset() override;

  shared_ptr<TcpSession> sessionPtr;
  Fn fn;
};