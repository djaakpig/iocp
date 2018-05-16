#pragma once
#include <windows.h>

class IoService final
{
public:
  bool Associate(HANDLE h);
  bool Start(const DWORD numWorkers);
  void Stop();

private:
  void _Run();

private:
  HANDLE _iocpHandle=nullptr;
};