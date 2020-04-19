#pragma once
#include "../Common.h"
#include "Kx/General/String.h"
#include "Kx/Localization/Locale.h"

namespace KxFramework::System::Private
{
	String FormatMessage(const void* source, uint32_t messageID, uint32_t flags = 0, const Locale& locale = {}) noexcept;

	String ResourceTypeToName(size_t id);
	String ResourceTypeToName(const wchar_t* id);
	const wchar_t* MakeIntResource(int resID);
}
