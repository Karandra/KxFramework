#include "KxStdAfx.h"
#include "LocalizationPack.h"
#include "Kx/General/XMLDocument.h"
#include "Kx/System/SystemInformation.h"
#include "KxFramework/KxLibrary.h"

namespace
{
	constexpr wxChar g_LibraryResourceType[] = wxS("Translation");
	
	static const KxFramework::LocalizationPack g_NullLocalizationPack;
	static const KxFramework::LocalizationPack* g_ActiveLocalizationPack = &g_NullLocalizationPack;

	KxFramework::Locale LocaleFromFileName(const KxFramework::String& name)
	{
		return name.BeforeFirst(wxS('.'));
	}
	bool DoLoadLocalizationPack(const KxFramework::XMLDocument& xml,
						 std::unordered_map<KxFramework::String, KxFramework::String>& stringTable,
						 KxFramework::String& author,
						 KxFramework::String& description
						 )
	{
		using namespace KxFramework;

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
	bool OnSearchTranslation(TFunc&& func, KxFramework::FileItem item)
	{
		using namespace KxFramework;

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

namespace KxFramework
{
	const LocalizationPack& LocalizationPack::GetActive() noexcept
	{
		return *g_ActiveLocalizationPack;
	}
	const LocalizationPack& LocalizationPack::SetActive(const LocalizationPack& localizationPack) noexcept
	{
		const LocalizationPack* previous = g_ActiveLocalizationPack;
		g_ActiveLocalizationPack = &localizationPack;
		return *previous;
	}

	bool LocalizationPack::Load(const String& xml, Locale locale)
	{
		m_Locale = std::move(locale);
		return DoLoadLocalizationPack(XMLDocument(xml), m_StringTable, m_Author, m_Description);
	}
	bool LocalizationPack::Load(wxInputStream& stream, Locale locale)
	{
		m_Locale = std::move(locale);
		return DoLoadLocalizationPack(XMLDocument(stream), m_StringTable, m_Author, m_Description);
	}
	bool LocalizationPack::Load(const KxLibrary& library, const FSPath& name, Locale locale)
	{
		if (library.IsOK())
		{
			if (UntypedMemorySpan data = library.GetResource(g_LibraryResourceType, name.GetName()))
			{
				if (!locale)
				{
					locale = LocaleFromFileName(name);
				}
				return Load(String::FromUTF8(reinterpret_cast<const char*>(data.data()), data.size()), std::move(locale));
			}
		}
		return false;
	}
}

namespace KxFramework::Localization
{
	size_t SearchLocalizationPacks(const IFileSystem& fileSystem, const FSPath& directory, std::function<bool(Locale, FileItem)> func)
	{
		return fileSystem.EnumItems(directory, [&](FileItem item)
		{
			return OnSearchTranslation(func, std::move(item));
		}, wxS("*.xml"), FSEnumItemsFlag::LimitToFiles);
	}
	size_t SearchLocalizationPacks(const KxLibrary& library, std::function<bool(Locale, FileItem)> func)
	{
		if (library.IsOK())
		{
			auto resourseList = library.EnumResources(g_LibraryResourceType);
			if (resourseList.empty())
			{
				size_t count = 0;
				for (const wxAny& any: resourseList)
				{
					count++;
					if (!OnSearchTranslation(func, FileItem(any.As<wxString>())))
					{
						break;
					}
				}
				return count;
			}
		}
		return 0;
	}
}
