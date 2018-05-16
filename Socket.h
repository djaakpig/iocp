#pragma once
#include <windows.h>
#include <winsock2.h>

class Socket final
{
public:
  bool Create(const int type, const int protocol);
  void Close();
  void GetExtention();
  HANDLE GetHandle() const
  {
    return reinterpret_cast<HANDLE>(_socket);
  }
  SOCKET GetSocket() const
  {
    return _socket;
  }
  bool SetOptionInt(const int level, const int name, int val);

private:
  SOCKET _socket=INVALID_SOCKET;
};