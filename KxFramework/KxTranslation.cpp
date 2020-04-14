#include "KxStdAfx.h"
#include "KxFramework/KxTranslation.h"
#include "KxFramework/KxLibrary.h"
#include "KxFramework/KxFileStream.h"
#include "Kx/FileSystem/NativeFileSystem.h"
#include "Kx/General/XMLDocument.h"
#include "Kx/System/SystemInformation.h"
#include "Kx/Localization/Common.h"
#include "Kx/Utility/Common.h"

namespace
{
	using namespace KxFramework;

	static wxString g_TranslationResourceType = wxS("Translation");
	
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
	LCTYPE lcType = KxFramework::System::IsWindows7OrGreater() ? LOCALE_SLOCALIZEDDISPLAYNAME : LOCALE_SLOCALIZEDLANGUAGENAME;

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
	using namespace KxFramework;

	KxStringToStringUMap translations;
	NativeFileSystem::Get().EnumItems(folderPath, [&](const FileItem& item)
	{
		// Extract locale name from names like 'en-US.Application.xml'
		wxString localeName = item.GetName().BeforeFirst(wxS('.'));

		// Check locale name
		if (!GetLanguageFullName(localeName).IsEmpty())
		{
			wxString name = item.GetName().BeforeLast(wxS('.'));
			if (!name.IsEmpty())
			{
				translations.insert(std::make_pair(name, item.GetFullPath().GetFullPath().GetWxString()));
			}
		}
		return true;
	}, wxS("*.xml"), FSEnumItemsFlag::LimitToFiles);
	return translations;
}
KxStringVector KxTranslation::FindTranslationsInResources()
{
	KxLibrary appLib(nullptr);
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
bool KxTranslation::Init(const KxFramework::XMLDocument& xml)
{
	using namespace KxFramework;

	if (xml)
	{
		if (XMLNode stringtTableNode = xml.QueryElement(wxS("Lang/StringTable")))
		{
			m_TranslatorName = xml.QueryElement(wxS("Lang/Info/Translator")).GetValue();
			stringtTableNode.EnumChildElements([&](XMLNode node)
			{
				String id = node.GetAttribute(wxS("ID"));
				if (!id.IsEmpty())
				{
					m_StringTable.emplace(id, node.GetValue().GetWxString());
				}
				return true;
			});
			return true;
		}
	}
	return false;
}
bool KxTranslation::LoadFromResourceInModule(const wxString& localeName, const KxLibrary& library)
{
	using namespace KxFramework;

	m_Locale = localeName;
	if (library.IsOK())
	{
		if (UntypedMemorySpan data = library.GetResource(g_TranslationResourceType, localeName))
		{
			KxFramework::XMLDocument xml(String::FromUTF8(reinterpret_cast<const char*>(data.data()), data.size()));
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
		Utility::SetIfNotNull(isSuccessOut, true);
		return it->second;
	}

	Utility::SetIfNotNull(isSuccessOut, false);
	return KxNullWxString;
}
wxString KxTranslation::GetString(wxStandardID id, bool* isSuccessOut) const
{
	String result = Localization::GetStandardLocalizedString(id);
	Utility::SetIfNotNull(isSuccessOut, !result.IsEmpty());

	return result;
}
wxString KxTranslation::GetString(KxStandardID id, bool* isSuccessOut) const
{
	return GetString(static_cast<wxStandardID>(id), isSuccessOut);
}

bool KxTranslation::LoadFromFile(const wxString& filePath)
{
	Clear();

	KxFileStream stream(filePath, KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Read);
	if (stream.IsOk())
	{
		return Init(KxFramework::XMLDocument(stream));
	}
	return false;
}
bool KxTranslation::LoadFromResource(const wxString& localeName)
{
	Clear();
	return LoadFromResourceInModule(localeName, KxLibrary(nullptr));
}
bool KxTranslation::LoadFromResource(const wxString& localeName, const KxLibrary& library)
{
	Clear();
	return LoadFromResourceInModule(localeName, library);
}
