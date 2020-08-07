#pragma once
#include "../Common.h"
#include "../Locale.h"

namespace kxf::Localization::Private
{
	constexpr XChar EmbeddedResourceType[] = wxS("LocalizationPackage");

	inline Locale LocaleFromFileName(const String& name)
	{
		return name.BeforeFirst(wxS('.'));
	}
}
