#pragma once
#include "../Common.h"
#include "../Locale.h"

namespace kxf::Localization::Private
{
	namespace EmbeddedResourceType
	{
		constexpr XChar Android[] = kxS("AndroidLocalizationPackage");
		constexpr XChar Windows[] = kxS("WindowsLocalizationPackage";)
		constexpr XChar Qt[] = kxS("QtLocalizationPackage");
	}

	inline Locale LocaleFromFileName(const String& name)
	{
		return name.BeforeFirst('.');
	}
}
