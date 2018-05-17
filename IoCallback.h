#pragma once
#include <Windows.h>
#include <WinSock2.h>
#include <memory>
#incluse <funcrional>
using namespace std;

struct IoCallback : OVERLAPPED
{
    IoCallback()
    {
        _Reset();
    }
    virtual ~IoCallback() = default;

    virtual bool OnComplete(const int e, const DWORD numBytes) = 0;
    virtual void Reset() = 0;

protected:
    void _Reset()
    {
      const auto pOvl = static_cast<LPOVERLAPPED>(this);
      SecureZeroMemory(pOvl, sizeof(OVERLAPPED));
    }
};