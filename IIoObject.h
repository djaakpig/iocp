#pragma once
#include <Windows.h>

__interface IIoObject
{
	HANDLE GetHandle() const;
};