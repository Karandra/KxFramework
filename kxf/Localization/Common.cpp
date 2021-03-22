#include "KxfPCH.h"
#include "Common.h"
#include "Locale.h"
#include "ILocalizationPackage.h"
#include "kxf/UI/WidgetID.h"
#include "kxf/General/Enumerator.h"
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
	String GetStandardString(const WidgetID& id)
	{
		return Private::LocalizeLabelString(FromInt<StdID>(*id));
	}

	size_t SearchPackages(const IFileSystem& fileSystem, const FSPath& directory, std::function<bool(Locale, FileItem)> func)
	{
		std::unordered_set<String> extensions;
		for (auto&& classInfo: RTTI::GetClassInfo<ILocalizationPackage>().EnumDerivedClasses())
		{
			if (auto instance = classInfo.CreateObjectInstance<ILocalizationPackage>())
			{
				for (auto&& ext: instance->EnumFileExtensions())
				{
					extensions.insert(ext.ToLower());
				}
			}
		}

		size_t count = 0;
		for (const FileItem& item: fileSystem.EnumItems(directory, {}, FSActionFlag::LimitToFiles))
		{
			if (extensions.find(item.GetFileExtension().MakeLower()) != extensions.end())
			{
				count++;
				if (!OnSearchPackage(func, std::move(item)))
				{
					break;
				}
			}
		}
		return count;
	}
	size_t SearchPackages(const DynamicLibrary& library, std::function<bool(Locale, FileItem)> func)
	{
		if (library)
		{
			using namespace Localization::Private;

			size_t count = 0;
			count += library.EnumResourceNames(EmbeddedResourceType::Android, [&](String name)
			{
				return OnSearchPackage(func, FileItem(std::move(name)));
			});
			count += library.EnumResourceNames(EmbeddedResourceType::Windows, [&](String name)
			{
				return OnSearchPackage(func, FileItem(std::move(name)));
			});
			count += library.EnumResourceNames(EmbeddedResourceType::Qt, [&](String name)
			{
				return OnSearchPackage(func, FileItem(std::move(name)));
			});

			return count;
		}
		return 0;
	}
}
