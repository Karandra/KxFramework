#include "KxStdAfx.h"
#include "KxFramework/KxTranslation.h"
#include "KxFramework/KxXML.h"
#include "KxFramework/KxSystem.h"
#include "KxFramework/KxLibrary.h"
#include "KxFramework/KxFileStream.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxFileFinder.h"
#include "KxFramework/KxXML.h"

namespace
{
	static wxString g_TranslationResourceType = "Translation";
	
	static const KxTranslation g_DefaultTranslation;
	static const KxTranslation* g_CurrentTranslation = &g_DefaultTranslation;
}

const KxTranslation& KxTranslation::GetCurrent()
{
	return *g_CurrentTranslation;
}
void KxTranslation::SetCurrent(const KxTranslation& translation)
{
	g_CurrentTranslation = translation.IsOK() ? &translation : &g_DefaultTranslation;
}

wxString KxTranslation::GetUserDefaultLocale()
{
	wchar_t name[LOCALE_NAME_MAX_LENGTH] = {0};
	::GetUserDefaultLocaleName(name, LOCALE_NAME_MAX_LENGTH);
	return name;
}
wxString KxTranslation::GetSystemDefaultLocale()
{
	wchar_t name[LOCALE_NAME_MAX_LENGTH] = {0};
	::GetSystemDefaultLocaleName(name, LOCALE_NAME_MAX_LENGTH);
	return name;
}
wxString KxTranslation::GetSystemPreferredLocale()
{
	ULONG langCount = 0;
	ULONG bufferSize = 0;
	if (!::GetSystemPreferredUILanguages(MUI_LANGUAGE_NAME, &langCount, nullptr, &bufferSize))
	{
		std::vector<wchar_t> buffer(bufferSize, L'\000');
		if (::GetSystemPreferredUILanguages(MUI_LANGUAGE_NAME, &langCount, buffer.data(), &bufferSize))
		{
			return buffer.data();
		}
	}
	return wxEmptyString;
}

wxString KxTranslation::GetLanguageFullName(const wxString& localeName)
{
	LCTYPE lcType = KxSystem::IsWindows7OrGreater() ? LOCALE_SLOCALIZEDDISPLAYNAME : LOCALE_SLOCALIZEDLANGUAGENAME;

	int length = ::GetLocaleInfoEx(localeName.wc_str(), lcType, nullptr, 0);
	if (length != 0)
	{
		wxString langName;
		::GetLocaleInfoEx(localeName.wc_str(), lcType, wxStringBuffer(langName, length), length);
		return langName;
	}
	return wxEmptyString;
}
wxString KxTranslation::LangIDToLocaleName(const LANGID& langID, DWORD sortOrder)
{
	return LCIDToLocaleName(MAKELCID(langID, sortOrder));
}
wxString KxTranslation::LCIDToLocaleName(const LCID& lcid)
{
	wchar_t name[LOCALE_NAME_MAX_LENGTH] = {0};
	::LCIDToLocaleName(lcid, name, LOCALE_NAME_MAX_LENGTH, 0);
	return name;
}

KxTranslation::AvailableMap KxTranslation::FindTranslationsInDirectory(const wxString& folderPath)
{
	KxStringToStringUMap translations;

	KxFileFinder finder(folderPath, wxS("*.xml"));
	for (KxFileItem item = finder.FindNext(); item.IsOK(); item = finder.FindNext())
	{
		if (item.IsFile())
		{
			// Extract locale name from names like 'en-US.Application.xml'
			wxString localeName = item.GetName().BeforeFirst(wxS('.'));

			// Check locale name
			if (!GetLanguageFullName(localeName).IsEmpty())
			{
				wxString name = item.GetName().BeforeLast(wxS('.'));
				if (!name.IsEmpty())
				{
					translations.insert(std::make_pair(name, item.GetFullPath()));
				}
			}
		}
	}
	return translations;
}
KxStringVector KxTranslation::FindTranslationsInResources()
{
	KxLibrary appLib(KxUtility::GetAppHandle());
	if (appLib.IsOK())
	{
		KxAnyVector resourseList = appLib.EnumResources(g_TranslationResourceType);

		KxStringVector localeNames;
		localeNames.reserve(resourseList.size());

		for (const wxAny& any: resourseList)
		{
			localeNames.push_back(any.As<wxString>());
		}
		return localeNames;
	}
	return KxStringVector();
}

void KxTranslation::Clear()
{
	m_StringTable.clear();
	m_TranslatorName.clear();
}
bool KxTranslation::Init(const KxXMLDocument& xml)
{
	if (xml.IsOK())
	{
		if (KxXMLNode stringtTableNode = xml.QueryElement(wxS("Lang/StringTable")))
		{
			m_TranslatorName = xml.QueryElement(wxS("Lang/Info/Translator")).GetValue();

			for (KxXMLNode node = stringtTableNode.GetFirstChildElement(); node; node = node.GetNextSiblingElement())
			{
				wxString id = node.GetAttribute("ID");
				if (!id.IsEmpty())
				{
					m_StringTable.emplace(id, node.GetValue());
				}
			}
			return true;
		}
	}
	return false;
}
bool KxTranslation::LoadFromResourceInModule(const wxString& localeName, const KxLibrary& library)
{
	m_Locale = localeName;
	if (library.IsOK())
	{
		KxUnownedMemoryBuffer data = library.GetResource(g_TranslationResourceType, localeName);
		if (!data.empty())
		{
			KxXMLDocument xml(wxString::FromUTF8((const char*)data.data(), data.size()));
			return Init(xml);
		}
	}
	return false;
}

KxTranslation::KxTranslation()
{
}
KxTranslation::~KxTranslation()
{
}

const wxString& KxTranslation::GetString(const wxString& id, bool* isSuccessOut) const
{
	auto it = m_StringTable.find(id);
	if (it != m_StringTable.end())
	{
		KxUtility::SetIfNotNull(isSuccessOut, true);
		return it->second;
	}

	KxUtility::SetIfNotNull(isSuccessOut, false);
	return KxNullWxString;
}
wxString KxTranslation::GetString(wxStandardID id, bool* isSuccessOut) const
{
	return KxUtility::GetStandardLocalizedString(id, isSuccessOut);
}
wxString KxTranslation::GetString(KxStandardID id, bool* isSuccessOut) const
{
	return KxUtility::GetStandardLocalizedString(id, isSuccessOut);
}

bool KxTranslation::LoadFromFile(const wxString& filePath)
{
	Clear();

	KxFileStream stream(filePath, KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Read);
	if (stream.IsOk())
	{
		return Init(KxXMLDocument(stream));
	}
	return false;
}
bool KxTranslation::LoadFromResource(const wxString& localeName)
{
	Clear();
	return LoadFromResourceInModule(localeName, KxLibrary(KxUtility::GetAppHandle()));
}
bool KxTranslation::LoadFromResource(const wxString& localeName, const KxLibrary& library)
{
	Clear();
	return LoadFromResourceInModule(localeName, library);
}
