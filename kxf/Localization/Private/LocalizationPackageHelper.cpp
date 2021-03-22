#include "KxfPCH.h"
#include "LocalizationPackageHelper.h"
#include "LocalizationResources.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/Serialization/XML.h"
#include "kxf/Utility/Enumerator.h"

namespace kxf::Localization::Private
{
	bool XMLPackageHelper::Load(IInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme)
	{
		if (stream && DoLoadXML(XMLDocument(stream), loadingScheme))
		{
			DoSetLocale(locale);
			return true;
		}
		return false;
	}
	bool XMLPackageHelper::Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme)
	{
		auto DoLoad = [&](const String& resType)
		{
			if (auto data = library.GetResource(resType, name.GetName()))
			{
				Locale usedLcoale = locale;
				if (!usedLcoale)
				{
					usedLcoale = Localization::Private::LocaleFromFileName(name);
				}
				if (DoLoadXML(String::FromUTF8(data), loadingScheme))
				{
					DoSetLocale(usedLcoale);
					return true;
				}
			}
			return false;
		};

		using namespace Localization::Private::EmbeddedResourceType;
		return DoLoad(Android) || DoLoad(Windows) || DoLoad(Qt);
	}

	Enumerator<ILocalizationPackage::ItemRef> ItemsPackageHelper::EnumItems() const
	{
		if (m_Items && !m_Items->empty())
		{
			return Utility::EnumerateIterableContainer<ILocalizationPackage::ItemRef>(*m_Items, [](const auto& it)
			{
				return ILocalizationPackage::ItemRef(it.first, it.second);
			});
		}
		return {};
	}
}
