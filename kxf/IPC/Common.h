#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/Common.h"

namespace kxf
{
	enum class RPCExchangeFlag: uint32_t
	{
		None = 0,

		GlobalSession = 1 << 0
	};
	KxFlagSet_Declare(RPCExchangeFlag);
}
