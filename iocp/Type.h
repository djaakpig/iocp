#pragma once
#include <stdint.h>

enum class ELogLevel : int32_t
{
	normal = 0,
	warning = 1,
	error = 2,
	disable = 3,
};

using PacketLength = uint32_t;
using SessionId = uint32_t;