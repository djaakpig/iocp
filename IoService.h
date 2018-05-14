#pragma once

class IoService
{
public:
  bool Associate(HANDLE h);
  bool Initialize();
  void Finalize();
private:
  HANDLE iocpHandle = NULL;
};