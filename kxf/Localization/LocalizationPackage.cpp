#include "stdafx.h"
#include "LocalizationPackage.h"
#include "Private/LocalizationResources.h"
#include "kxf/Serialization/XML.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/System/DynamicLibrary.h"

namespace
{
	constexpr wxChar g_EmbeddedResourceType[] = wxS("Translation");
	
	const kxf::LocalizationPackage g_NullLocalizationPack;
	const kxf::LocalizationPackage* g_ActiveLocalizationPack = &g_NullLocalizationPack;

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
			if (auto data = library.GetResource(Localization::Private::EmbeddedResourceType, name.GetName()))
			{
				Locale usedLcoale = locale;
				if (!usedLcoale)
				{
					usedLcoale = Localization::Private::LocaleFromFileName(name);
				}
				return Load(String::FromUTF8(data), std::move(usedLcoale));
			}
		}
		return false;
	}
}
