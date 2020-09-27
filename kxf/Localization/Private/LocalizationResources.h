#pragma once
#include "../Common.h"
#include "../Locale.h"

namespace kxf::Localization::Private
{
	namespace EmbeddedResourceType
	{
		constexpr XChar Android[] = wxS("AndroidLocalizationPackage");
		constexpr XChar Windows[] = wxS("WindowsLocalizationPackage");
		constexpr XChar Qt[] = wxS("QtLocalizationPackage");
	}

	inline Locale LocaleFromFileName(const String& name)
	{
		return name.BeforeFirst(wxS('.'));
	}
}
