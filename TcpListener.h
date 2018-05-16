#pragma once
#include "IIoObject.h"

class Socket;
class TcpListener final : public IIoObject
{
public:
  TcpListener();
  ~TcpListener();

  HANDLE GetHandle() const override;
  bool Start(const string& ip, const WORD port);
  void Stop();

private:
  Socket* _pSocket=nullptr;
};