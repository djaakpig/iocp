#pragma once

class IoService
{
public:
  bool Associate(HANDLE h);
  bool Initialize();
  void Finalize();
private:
  void _Run();
private:
  HANDLE iocpHandle = nullptr;
};