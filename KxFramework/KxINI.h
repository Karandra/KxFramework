/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
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
	private:
		using DocumentT = SimpleINI::CSimpleIniA;
		DocumentT m_Document;

	private:
		static wxString ToWxString(const std::string& s)
		{
			return wxString::FromUTF8Unchecked(s.c_str(), s.size());
		}
		static wxString ToWxString(const char* s)
		{
			return wxString::FromUTF8Unchecked(s);
		}

		bool Load(const char* xmlText, size_t length);
		void UnLoad();

	private:
		/* General */
		KxINI(const KxINI& other)
		{
			// Invalid
		}
		virtual Node QueryElement(const wxString& XPath) const override
		{
			return Node();
		}
		virtual Node QueryOrCreateElement(const wxString& XPath) override
		{
			return Node();
		}

		/* Node */
		virtual size_t GetIndexWithinParent() const override
		{
			return 0;
		}
		virtual wxString GetName() const override
		{
			return wxEmptyString;
		}

		virtual size_t GetChildrenCount() const override
		{
			return 0;
		}
		virtual NodeVector GetChildren() const override
		{
			return NodeVector();
		}

		/* Values */
		virtual wxString GetValue(const wxString& defaultValue = wxEmptyString) const override
		{
			return wxEmptyString;
		}
		virtual bool DoSetValue(const wxString& value, bool isCDATA = false) override
		{
			return false;
		}
		
		wxString DoGetValue(const wxString& sectionName, const wxString& keyName, const wxString& defaultValue = wxEmptyString) const;
		bool DoSetValue(const wxString& sectionName, const wxString& keyName, const wxString& value);

		/* Attributes */
		virtual bool DoSetAttribute(const wxString& name, const wxString& value) override
		{
			return false;
		}
		virtual size_t GetAttributeCount() const override
		{
			return 0;
		}
		virtual KxStringVector GetAttributes() const override
		{
			return KxStringVector();
		}
		virtual bool HasAttribute(const wxString& name) const override
		{
			return false;
		}
		virtual wxString GetAttribute(const wxString& name, const wxString& defaultValue = wxEmptyString) const override
		{
			return wxEmptyString;
		}

		/* Navigation */
		virtual Node GetElementByAttribute(const wxString& name, const wxString& value) const override
		{
			return Node();
		}
		virtual Node GetElementByTag(const wxString& tagName) const override
		{
			return Node();
		}

		virtual Node GetParent() const override
		{
			return Node();
		}
		virtual Node GetPreviousSibling() const override
		{
			return Node();
		}
		virtual Node GetNextSibling() const override
		{
			return Node();
		}
		virtual Node GetFirstChild() const override
		{
			return Node();
		}
		virtual Node GetLastChild() const override
		{
			return Node();
		}

	public:
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
			return DoGetValue(sectionName, keyName, defaultValue);
		}
		int64_t GetValueInt(const wxString& sectionName, const wxString& keyName, int64_t defaultValue = 0) const
		{
			return ParseInt(DoGetValue(sectionName, keyName), defaultValue);
		}
		double GetValueFloat(const wxString& sectionName, const wxString& keyName, double defaultValue = 0.0) const
		{
			return ParseFloat(DoGetValue(sectionName, keyName), defaultValue);
		}
		bool GetValueBool(const wxString& sectionName, const wxString& keyName, bool defaultValue = false) const
		{
			return ParseBool(DoGetValue(sectionName, keyName), defaultValue);
		}

		bool SetValue(const wxString& sectionName, const wxString& keyName, const wxString& value)
		{
			return DoSetValue(sectionName, keyName, value);
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, const char* value)
		{
			return DoSetValue(sectionName, keyName, wxString::FromUTF8(value));
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, const wchar_t* value)
		{
			return DoSetValue(sectionName, keyName, wxString(value));
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, int64_t value)
		{
			return DoSetValue(sectionName, keyName, FormatInt(value));
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, int value)
		{
			return DoSetValue(sectionName, keyName, FormatInt(value));
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, double value, int precision = -1)
		{
			return DoSetValue(sectionName, keyName, FormatFloat(value, precision));
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, float value, int precision = -1)
		{
			return DoSetValue(sectionName, keyName, FormatFloat((double)value, precision));
		}
		bool SetValue(const wxString& sectionName, const wxString& keyName, bool value)
		{
			return DoSetValue(sectionName, keyName, FormatBool(value));
		}

		/* Deletion */
		bool RemoveSection(const wxString& sectionName);
		bool RemoveValue(const wxString& sectionName, const wxString& keyName);
};
