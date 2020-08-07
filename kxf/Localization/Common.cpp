#include "stdafx.h"
#include "Common.h"
#include "Locale.h"
#include "kxf/System/DynamicLibrary.h"
#include "Private/StandardLocalization.h"
#include "Private/LocalizationResources.h"

namespace
{
	template<class TFunc>
	bool OnSearchPackage(TFunc&& func, kxf::FileItem item)
	{
		using namespace kxf;

		// Extract locale name from names like 'en-US.Application.xml'
		if (Locale locale = Localization::Private::LocaleFromFileName(item.GetName()))
		{
			String name = item.GetName().BeforeLast(wxS('.'));
			if (!name.IsEmpty())
			{
				return std::invoke(func, std::move(locale), std::move(item));
			}
		}
		return true;
	}
}

namespace kxf::Localization
{
	String GetStandardString(int id)
	{
		return Private::LocalizeLabelString(FromInt<StdID>(id));
	}
	String GetStandardString(StdID id)
	{
		return Private::LocalizeLabelString(id);
	}

	size_t SearchPackages(const IFileSystem& fileSystem, const FSPath& directory, std::function<bool(Locale, FileItem)> func)
	{
		return fileSystem.EnumItems(directory, [&](FileItem item)
		{
			// TODO: Need a proper solution instead of manually adding supported file extensions here.
			String name = item.GetName();
			if (name.MatchesWildcards(wxS("*.xml")) || name.MatchesWildcards(wxS("*.resx")) || name.MatchesWildcards(wxS("*.resw")))
			{
				return OnSearchPackage(func, std::move(item));
			}
			return true;
		}, {}, FSActionFlag::LimitToFiles);
	}
	size_t SearchPackages(const DynamicLibrary& library, std::function<bool(Locale, FileItem)> func)
	{
		if (library)
		{
			return library.EnumResourceNames(Private::EmbeddedResourceType, [&](String name)
			{
				return OnSearchPackage(func, FileItem(std::move(name)));
			});
		}
		return 0;
	}
}
