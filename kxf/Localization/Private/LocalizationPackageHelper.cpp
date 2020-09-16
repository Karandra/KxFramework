#include "stdafx.h"
#include "LocalizationPackageHelper.h"
#include "LocalizationResources.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/Serialization/XML.h"

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
		if (auto data = library.GetResource(Localization::Private::EmbeddedResourceType, name.GetName()))
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
	}
}
