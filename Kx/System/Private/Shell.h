#pragma once
#include "../Common.h"

namespace KxFramework::Shell::Private
{
	std::optional<uint32_t> MapSHWindowCommand(SHWindowCommand option) noexcept;
	SHWindowCommand MapSHWindowCommand(uint32_t option) noexcept;
}
