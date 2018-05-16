#pragma once
#include <windows.h>

__interface IIoObject
{
  HANDLE GetHandle() const;
};