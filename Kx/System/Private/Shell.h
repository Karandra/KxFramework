#pragma once
#include "../Common.h"

namespace KxFramework::Shell::Private
{
	std::optional<uint32_t> MapWindowVisibilityOption(WindowVisibilityOption option) noexcept;
	WindowVisibilityOption MapWindowVisibilityOption(uint32_t option) noexcept;
}
