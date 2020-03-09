#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxXDocumentNode.h"

namespace SimpleINI
{
	#define _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING 1
	#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 1

	#include "KxFramework/INI/SimpleINI.h"
}

class KX_API KxINI: private KxXDocumentNode<KxINI>
{
	public:
		static wxString GetLibraryName();
		static wxString GetLibraryVersion();

	private:
		using TDocument = SimpleINI::CSimpleIniA;

	protected:
		static wxString ToWxString(const std::string& s)
		{
			return wxString::FromUTF8Unchecked(s.c_str(), s.size());
		}
		static wxString ToWxString(const char* s)
		{
			return wxString::FromUTF8Unchecked(s);
		}

	private:
		SimpleINI::CSimpleIniA m_Document;

	private:
		void Init();
		bool Load(const char* xmlText, size_t length);
		void UnLoad();

	protected:
		wxString DoGetValue(const wxString& defaultValue = wxEmptyString) const override;
		bool DoSetValue(const wxString& value, WriteEmpty writeEmpty = WriteEmpty::Always, AsCDATA asCDATA = AsCDATA::Auto) override;

		wxString DoGetAttribute(const wxString& name, const wxString& defaultValue = wxEmptyString) const override;
		bool DoSetAttribute(const wxString& name, const wxString& value, WriteEmpty writeEmpty = WriteEmpty::Always) override;

		wxString IniGetValue(const wxString& sectionName, const wxString& keyName, const wxString& defaultValue = wxEmptyString) const;
		bool IniSetValue(const wxString& sectionName, const wxString& keyName, const wxString& value, WriteEmpty writeEmpty = WriteEmpty::Always);

	public:
		KxINI(const KxINI&) = delete;
		KxINI(const wxString& iniText = wxEmptyString);
		KxINI(wxInputStream& stream);
		virtual ~KxINI();

	public:
		/* General */
		virtual bool IsOK() const override;

		bool Load(const wxString& iniText);
		bool Load(wxInputStream& stream);
		bool Save(wxOutputStream& stream) const;
		wxString Save() const;

		/* Navigation */
		KxStringVector GetSectionNames() const;
		KxStringVector GetKeyNames(const wxString& sectionName) const;

		/* Value */
		bool HasSection(const wxString& sectionName) const;
		bool HasValue(const wxString& sectionName, const wxString& keyName) const;

		wxString GetValue(const wxString& sectionName, const wxString& keyName, const wxString& defaultValue = wxEmptyString) const
		{
			return IniGetValue(sectionName, keyName, defaultValue);
		}
		int64_t GetValueInt(const wxString& sectionName, const wxString& keyName, int64_t defaultValue = 0) const
		{
			return GetValueIntWithBase(sectionName, keyName, 10, defaultValue);
		}
		int64_t GetValueIntWithBase(const wxString& sectionName, const wxString& keyName, int base, int64_t defaultValue = 0) const
		{
			return ParseInt(IniGetValue(sectionName, keyName), base, defaultValue);
		}
		double GetValueFloat(const wxString& sectionName, const wxString& keyName, double defaultValue = 0.0) const
		{
			return ParseFloat(IniGetValue(sectionName, keyName), defaultValue);
		}
		bool GetValueBool(const wxString& sectionName, const wxString& keyName, bool defaultValue = false) const
		{
			return ParseBool(IniGetValue(sectionName, keyName), defaultValue);
		}

		bool SetValue(const wxString& sectionName, const wxString& keyName, const wxString& value, WriteEmpty writeEmpty = WriteEmpty::Always)
		{
			return IniSetValue(sectionName, keyName, value, writeEmpty);
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, const char* value, WriteEmpty writeEmpty = WriteEmpty::Always)
		{
			return IniSetValue(sectionName, keyName, wxString::FromUTF8(value), writeEmpty);
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, const wchar_t* value, WriteEmpty writeEmpty = WriteEmpty::Always)
		{
			return IniSetValue(sectionName, keyName, wxString(value), writeEmpty);
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, int64_t value)
		{
			return IniSetValue(sectionName, keyName, FormatInt(value), WriteEmpty::Always);
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, int value)
		{
			return IniSetValue(sectionName, keyName, FormatInt(value), WriteEmpty::Always);
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, double value, int precision = -1)
		{
			return IniSetValue(sectionName, keyName, FormatFloat(value, precision), WriteEmpty::Always);
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, float value, int precision = -1)
		{
			return IniSetValue(sectionName, keyName, FormatFloat((double)value, precision), WriteEmpty::Always);
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, bool value)
		{
			return IniSetValue(sectionName, keyName, FormatBool(value), WriteEmpty::Always);
		}

		/* Deletion */
		bool RemoveSection(const wxString& sectionName);
		bool RemoveValue(const wxString& sectionName, const wxString& keyName);
};
