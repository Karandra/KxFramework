#include "stdafx.h"
#include "LocalizationPackage.h"
#include "kxf/Serialization/XML.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/System/DynamicLibrary.h"

namespace
{
	constexpr wxChar g_EmbeddedResourceType[] = wxS("Translation");
	
	const kxf::LocalizationPackage g_NullLocalizationPack;
	const kxf::LocalizationPackage* g_ActiveLocalizationPack = &g_NullLocalizationPack;

	kxf::Locale LocaleFromFileName(const kxf::String& name)
	{
		return name.BeforeFirst(wxS('.'));
	}
	bool DoLoadLocalizationPack(const kxf::XMLDocument& xml,
						 std::unordered_map<kxf::String, kxf::String>& stringTable,
						 kxf::String& author,
						 kxf::String& description
						 )
	{
		using namespace kxf;

		if (XMLNode rootNode = xml.QueryElement(wxS("Localization")))
		{
			author = rootNode.GetFirstChildElement(wxS("Author")).GetValue();
			description = rootNode.GetFirstChildElement(wxS("Description")).GetValue();

			if (XMLNode tableNode = rootNode.GetFirstChildElement(wxS("StringTable")))
			{
				size_t count = 0;
				tableNode.EnumChildElements([&](XMLNode node)
				{
					String id = node.GetAttribute(wxS("ID"));
					if (!id.IsEmpty() && stringTable.emplace(std::move(id), node.GetValue()).second)
					{
						count++;
					}
					return true;
				});
				return count;
			}
		}
		return false;
	}

	template<class TFunc>
	bool OnSearchTranslation(TFunc&& func, kxf::FileItem item)
	{
		using namespace kxf;

		// Extract locale name from names like 'en-US.Application.xml'
		if (Locale locale = LocaleFromFileName(item.GetName()))
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

namespace kxf
{
	const LocalizationPackage& LocalizationPackage::GetActive() noexcept
	{
		return *g_ActiveLocalizationPack;
	}
	const LocalizationPackage& LocalizationPackage::SetActive(const LocalizationPackage& package) noexcept
	{
		const LocalizationPackage* previous = g_ActiveLocalizationPack;
		g_ActiveLocalizationPack = &package;
		return *previous;
	}

	bool LocalizationPackage::Load(const String& xml, const Locale& locale)
	{
		m_Locale = locale;
		return DoLoadLocalizationPack(XMLDocument(xml), m_StringTable, m_Author, m_Description);
	}
	bool LocalizationPackage::Load(wxInputStream& stream, const Locale& locale)
	{
		m_Locale = locale;
		return DoLoadLocalizationPack(XMLDocument(stream), m_StringTable, m_Author, m_Description);
	}
	bool LocalizationPackage::Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale)
	{
		if (library)
		{
			if (auto data = library.GetResource(g_EmbeddedResourceType, name.GetName()))
			{
				Locale usedLcoale = locale;
				if (!usedLcoale)
				{
					usedLcoale = LocaleFromFileName(name);
				}
				return Load(String::FromUTF8(data), std::move(usedLcoale));
			}
		}
		return false;
	}
}

namespace kxf::Localization
{
	size_t SearchLocalizationPackages(const IFileSystem& fileSystem, const FSPath& directory, std::function<bool(Locale, FileItem)> func)
	{
		return fileSystem.EnumItems(directory, [&](FileItem item)
		{
			return OnSearchTranslation(func, std::move(item));
		}, wxS("*.xml"), FSActionFlag::LimitToFiles);
	}
	size_t SearchLocalizationPackages(const DynamicLibrary& library, std::function<bool(Locale, FileItem)> func)
	{
		if (library)
		{
			return library.EnumResourceNames(g_EmbeddedResourceType, [&](String name)
			{
				return OnSearchTranslation(func, FileItem(std::move(name)));
			});
		}
		return 0;
	}
}
