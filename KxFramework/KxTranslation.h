#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxXML.h"

using KxTranslationTable = std::unordered_map<wxString, wxString>;
class KxTranslation
{
	private:
		KxTranslation() = delete;
		KxTranslation& operator=(const KxTranslation&) = delete;

	private:
		static void InitStringTable();
		static void ClearStringTable();

	public:
		static KxTranslationTable FindTranslationsInDirectory(const wxString& folderPath);
		static KxStringVector FindTranslationsInResources();
		
		static bool LoadTranslationFromFile(const wxString& filePath);
		static bool LoadTranslationFromResource(const wxString& localeName);
		
		static const wxString& GetString(const wxString& id, bool* isSuccessOut = NULL);
		static const wxString& GetString(const char* id, bool* isSuccessOut = NULL)
		{
			return GetString(wxString::FromUTF8Unchecked(id), isSuccessOut);
		}
		static wxString GetString(wxStandardID id, bool* isSuccessOut = NULL)
		{
			return KxUtility::GetStandardLocalizedString(id, isSuccessOut);
		}
		static wxString GetString(KxStandardID id, bool* isSuccessOut = NULL)
		{
			return KxUtility::GetStandardLocalizedString(id, isSuccessOut);
		}

		static wxString GetUserDefaultLocale();
		static wxString GetSystemDefaultLocale();
		static wxString GetSystemPreferredLocale();

		static wxString GetLanguageFullName(const wxString& localeName);
		static wxString ToLocaleName(const LANGID& langID, DWORD sortOrder = SORT_DEFAULT);
		static wxString ToLocaleName(const LCID& lcid);
};
