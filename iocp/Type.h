#pragma once
#include <Windows.h>

enum class ELogLevel : int
{
	normal = 0,
	warning = 1,
	error = 2,
	disable = 3,
};

using PacketLength = DWORD;
using SessionId = DWORD;