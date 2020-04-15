#pragma once
#include "../Common.h"

namespace KxFramework::Localization::Private
{
	std::optional<uint32_t> MapLocaleOption(LocaleStrOption option) noexcept;
	std::optional<uint32_t> MapLocaleOption(LocaleIntOption option) noexcept;
}
