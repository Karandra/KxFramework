#include "KxStdAfx.h"
#include "KxFramework/KxTranslation.h"
#include "KxFramework/KxXML.h"
#include "KxFramework/KxSystem.h"
#include "KxFramework/KxLibrary.h"
#include "KxFramework/KxFileStream.h"
#include "KxFramework/KxFile.h"
#include "KxFramework/KxUtility.h"

namespace
{
	static KxXMLDocument g_CurrentTranslation;
	static std::unordered_map<wxString, wxString> g_StringTable;

	static wxString g_TranslationResourceType = "Translation";
}

void KxTranslation::InitStringTable()
{
	g_StringTable.clear();

	g_CurrentTranslation.SetXPathSeparator(wxEmptyString);
	if (g_CurrentTranslation.IsOK())
	{
		KxXMLNode element = g_CurrentTranslation.QueryElement("Lang/StringTable");
		if (element.IsOK())
		{
			element = element.GetFirstChild();
			while (element.IsOK())
			{
				wxString id = element.GetAttribute("ID");
				if (!id.IsEmpty())
				{
					g_StringTable.insert(std::make_pair(id, element.GetValue()));
				}

				element = element.GetNextSibling();
			}
		}
	}
}
void KxTranslation::ClearStringTable()
{
	g_StringTable.clear();
	g_CurrentTranslation.Load(wxEmptyString);
}

KxTranslationTable KxTranslation::FindTranslationsInDirectory(const wxString& folderPath)
{
	KxTranslationTable translations;
	KxStringVector filesList = KxFile(folderPath).Find("*.xml", KxFS_FILE);
	if (!filesList.empty())
	{
		for (const KxFile& filePath: filesList)
		{
			wxString localeName = filePath.GetName();
			
			// Check locale name
			if (!GetLanguageFullName(localeName).IsEmpty())
			{
				translations.insert(std::make_pair(localeName, filePath.GetFullPath()));
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

bool KxTranslation::LoadTranslationFromFile(const wxString& filePath)
{
	ClearStringTable();

	KxFileStream file(filePath, KxFS_ACCESS_READ, KxFS_DISP_OPEN_EXISTING, KxFS_SHARE_READ);
	if (file.IsOk() && g_CurrentTranslation.Load(file))
	{
		InitStringTable();
		return true;
	}
	return false;
}
bool KxTranslation::LoadTranslationFromResource(const wxString& localeName)
{
	ClearStringTable();

	KxLibrary appLib(KxUtility::GetAppHandle());
	if (appLib.IsOK())
	{
		wxMemoryBuffer data = appLib.GetResource(g_TranslationResourceType, localeName);
		if (!data.IsEmpty())
		{
			g_CurrentTranslation.Load(wxString::FromUTF8((const char*)data.GetData(), data.GetDataLen()));
			InitStringTable();

			return g_CurrentTranslation.IsOK();
		}
	}
	return false;
}

const wxString& KxTranslation::GetString(const wxString& id, bool* isSuccessOut)
{
	auto it = g_StringTable.find(id);
	if (it != g_StringTable.end())
	{
		KxUtility::SetIfNotNull(isSuccessOut, true);
		return it->second;
	}

	KxUtility::SetIfNotNull(isSuccessOut, false);
	return wxNullString;
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
	if (!::GetSystemPreferredUILanguages(MUI_LANGUAGE_NAME, &langCount, NULL, &bufferSize))
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

	int length = ::GetLocaleInfoEx(localeName, lcType, NULL, 0);
	if (length != 0)
	{
		wxString langName;
		::GetLocaleInfoEx(localeName, lcType, wxStringBuffer(langName, length), length);
		return langName;
	}
	return wxEmptyString;
}
wxString KxTranslation::ToLocaleName(const LANGID& langID, DWORD sortOrder)
{
	return ToLocaleName(MAKELCID(langID, sortOrder));
}
wxString KxTranslation::ToLocaleName(const LCID& lcid)
{
	wchar_t name[LOCALE_NAME_MAX_LENGTH] = {0};
	::LCIDToLocaleName(lcid, name, LOCALE_NAME_MAX_LENGTH, 0);
	return name;
}
