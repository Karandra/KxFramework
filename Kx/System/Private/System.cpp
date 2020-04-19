#include "stdafx.h"
#include "System.h"
#include "Kx/Utility/CallAtScopeExit.h"

namespace KxFramework::System::Private
{
	String FormatMessage(const void* source, uint32_t messageID, uint32_t flags, const Locale& locale) noexcept
	{
		auto lcid = (locale && !locale.IsInvariant() ? locale : locale.GetUserDefault()).GetLCID();

		wchar_t* formattedMessage = nullptr;
		uint32_t length = ::FormatMessageW(flags|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS,
										   source,
										   messageID,
										   lcid.value_or(MAKELCID(LANG_NEUTRAL, SORT_DEFAULT)),
										   reinterpret_cast<wchar_t*>(&formattedMessage),
										   0,
										   nullptr
		);
		if (length != 0 && formattedMessage)
		{
			Utility::CallAtScopeExit atExit([&]()
			{
				::LocalFree(formattedMessage);
			});
			return String(formattedMessage, length);
		}
		return {};
	}

	String ResourceTypeToName(size_t id)
	{
		wchar_t buffer[64] = {};
		swprintf_s(buffer, L"%zu", id);

		return buffer;
	}
	String ResourceTypeToName(const wchar_t* id)
	{
		return ResourceTypeToName(reinterpret_cast<size_t>(id));
	}
	const wchar_t* MakeIntResource(int resID)
	{
		return MAKEINTRESOURCEW(resID);
	}
}
