#pragma once
#include "IIoObject.h"

class Socket;
class TcpSession final : public IIoObject
{
public:
  void Close();
  bool Create();
  HANDLE GetHandle() const override;
  bool Recv();
  bool Send();

private:
  Socket* _pSocket=nullptr;
};