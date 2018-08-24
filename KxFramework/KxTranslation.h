#pragma once
#include "KxFramework/KxFramework.h"
class KxXMLDocument;
class KxLibrary;

class KxTranslation
{
	public:
		static const KxTranslation& GetCurrent();
		static void SetCurrent(const KxTranslation& translation);

	public:
		static wxString GetUserDefaultLocale();
		static wxString GetSystemDefaultLocale();
		static wxString GetSystemPreferredLocale();

		static wxString GetLanguageFullName(const wxString& localeName);
		static wxString LangIDToLocaleName(const LANGID& langID, DWORD sortOrder = SORT_DEFAULT);
		static wxString LCIDToLocaleName(const LCID& lcid);

		static KxStringToStringUMap FindTranslationsInDirectory(const wxString& folderPath);
		static KxStringVector FindTranslationsInResources();

	private:
		std::unordered_map<wxString, wxString> m_StringTable;
		wxString m_TranslatorName;

	private:
		void Clear();
		bool Init(const KxXMLDocument& xml);
		bool LoadFromResourceInModule(const wxString& localeName, const KxLibrary& library);

	public:
		KxTranslation();
		virtual ~KxTranslation();

	public:
		bool IsOK() const
		{
			return !m_StringTable.empty();
		}
		const wxString& GetTranslatorName() const
		{
			return m_TranslatorName;
		}

		const wxString& GetString(const wxString& id, bool* isSuccessOut = NULL) const;
		const wxString& GetString(const char* id, bool* isSuccessOut = NULL) const
		{
			return GetString(wxString::FromUTF8Unchecked(id), isSuccessOut);
		}
		wxString GetString(wxStandardID id, bool* isSuccessOut = NULL) const;
		wxString GetString(KxStandardID id, bool* isSuccessOut = NULL) const;

		bool LoadFromFile(const wxString& filePath);
		bool LoadFromResource(const wxString& localeName);
		bool LoadFromResource(const wxString& localeName, const KxLibrary& library);
};
