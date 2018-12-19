/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
class KxXMLDocument;
class KxLibrary;

class KX_API KxTranslation
{
	public:
		using StringsMap = std::unordered_map<wxString, wxString>;
		using AvailableMap = std::unordered_map<wxString, wxString>;

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

		static AvailableMap FindTranslationsInDirectory(const wxString& folderPath);
		static KxStringVector FindTranslationsInResources();

	private:
		StringsMap m_StringTable;
		wxString m_Locale;
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
		
		bool IsLocaleKnown() const
		{
			return !m_Locale.IsEmpty();
		}
		wxString GetLocale() const
		{
			return m_Locale;
		}
		void SetLocale(const wxString& value)
		{
			m_Locale = value;
		}
		wxString GetTranslatorName() const
		{
			return m_TranslatorName;
		}

		const wxString& GetString(const wxString& id, bool* isSuccessOut = nullptr) const;
		const wxString& GetString(const char* id, bool* isSuccessOut = nullptr) const
		{
			return GetString(wxString::FromUTF8Unchecked(id), isSuccessOut);
		}
		wxString GetString(wxStandardID id, bool* isSuccessOut = nullptr) const;
		wxString GetString(KxStandardID id, bool* isSuccessOut = nullptr) const;

		bool LoadFromFile(const wxString& filePath);
		bool LoadFromResource(const wxString& localeName);
		bool LoadFromResource(const wxString& localeName, const KxLibrary& library);
};
