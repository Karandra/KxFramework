#pragma once
#include "KxFramework/KxFramework.h"

namespace KxXDocumentNodeInternal
{
	bool ParseBool(const wxString& value, bool defaultValue);
	int ExtractIndexFromName(wxString& elementName, const wxString& XPathDelimiter = wxEmptyString);
}

template<class NodeT>
class KxXDocumentNode
{
	public:
		using Node = NodeT;
		using NodeVector = std::vector<Node>;

	public:
		static int ExtractIndexFromName(wxString& elementName, const wxString& XPathDelimiter = wxEmptyString)
		{
			return KxXDocumentNodeInternal::ExtractIndexFromName(elementName, XPathDelimiter);
		}

	protected:
		virtual wxString FormatInt(int64_t value) const
		{
			return wxString::Format("%lld", value);
		}
		virtual wxString FormatFloat(double value, int precision = -1) const
		{
			return wxString::FromCDouble(value, precision);
		}
		virtual wxString FormatBool(bool value) const
		{
			return value ? "1" : "0";
		}

		virtual int64_t ParseInt(const wxString& value, int64_t defaultValue = 0) const
		{
			int64_t iValue = defaultValue;
			if (value.ToLongLong(&iValue))
			{
				return iValue;
			}
			return defaultValue;
		}
		virtual double ParseFloat(const wxString& value, double defaultValue = 0.0) const
		{
			double dValue = defaultValue;
			if (value.ToCDouble(&dValue))
			{
				return dValue;
			}
			return defaultValue;
		}
		virtual bool ParseBool(const wxString& value, bool defaultValue = false) const
		{
			return KxXDocumentNodeInternal::ParseBool(value, defaultValue);
		}

		virtual bool DoSetValue(const wxString& value, bool isCDATA = false) = 0;
		virtual bool DoSetAttribute(const wxString& name, const wxString& value) = 0;

	public:
		/* General */
		virtual bool IsOK() const = 0;
		virtual Node QueryElement(const wxString& XPath) const = 0;

		/* Node */
		virtual size_t GetIndexWithinParent() const = 0;
		virtual wxString GetName() const = 0;
		virtual bool SetName(const wxString& name)
		{
			return false;
		}

		virtual size_t GetChildrenCount() const = 0;
		virtual bool HasChildren() const
		{
			return GetChildrenCount() != 0;
		}
		virtual NodeVector GetChildren() const = 0;
		virtual bool ClearChildren()
		{
			return false;
		}
		virtual bool ClearNode()
		{
			return false;
		}

		/* Values */
		virtual wxString GetValue(const wxString& defaultValue = wxEmptyString) const = 0;
		virtual int64_t GetValueInt(int64_t defaultValue) const
		{
			return ParseInt(GetValue(), defaultValue);
		}
		virtual double GetValueFloat(double defaultValue) const
		{
			return ParseFloat(GetValue(), defaultValue);
		}
		virtual bool GetValueBool(bool defaultValue) const
		{
			return ParseBool(GetValue(), defaultValue);
		}

		bool SetValue(const wxString& value, bool isCDATA = false)
		{
			return DoSetValue(value, isCDATA);
		}
		bool SetValue(const char* value, bool isCDATA = false)
		{
			return DoSetValue(wxString::FromUTF8(value), isCDATA);
		}
		bool SetValue(const wchar_t* value, bool isCDATA = false)
		{
			return DoSetValue(wxString(value), isCDATA);
		}
		bool SetValue(int64_t value, bool isCDATA = false)
		{
			return DoSetValue(FormatInt(value), isCDATA);
		}
		bool SetValue(int value, bool isCDATA = false)
		{
			return DoSetValue(FormatInt(value), isCDATA);
		}
		bool SetValue(double value, int precision = -1, bool isCDATA = false)
		{
			return DoSetValue(FormatFloat(value, precision), isCDATA);
		}
		bool SetValue(float value, int precision = -1, bool isCDATA = false)
		{
			return DoSetValue(FormatFloat((double)value, precision), isCDATA);
		}
		bool SetValue(bool value, bool isCDATA = false)
		{
			return DoSetValue(FormatBool(value), isCDATA);
		}

		virtual bool IsCDATA() const
		{
			return false;
		}
		virtual bool SetCDATA(bool value)
		{
			return false;
		}

		/* Attributes */
		virtual size_t GetAttributeCount() const = 0;
		virtual bool HasAttributes() const
		{
			return GetAttributeCount() != 0;
		}
		virtual KxStringVector GetAttributes() const = 0;
		
		virtual bool HasAttribute(const wxString& name) const = 0;
		virtual bool RemoveAttribute(const wxString& name)
		{
			return false;
		}
		virtual bool ClearAttributes()
		{
			return false;
		}

		virtual wxString GetAttribute(const wxString& name, const wxString& defaultValue = wxEmptyString) const = 0;
		virtual int64_t GetAttributeInt(const wxString& name, int64_t defaultValue = 0) const
		{
			return ParseInt(GetAttribute(name), defaultValue);
		}
		virtual double GetAttributeFloat(const wxString& name, double defaultValue = 0.0) const
		{
			return ParseFloat(GetAttribute(name), defaultValue);
		}
		virtual bool GetAttributeBool(const wxString& name, bool defaultValue = false) const
		{
			return ParseBool(GetAttribute(name), defaultValue);
		}

		bool SetAttribute(const wxString& name, const wxString& value)
		{
			return DoSetAttribute(name, value);
		}
		bool SetAttribute(const wxString& name, const char* value)
		{
			return DoSetAttribute(name, wxString::FromUTF8(value));
		}
		bool SetAttribute(const wxString& name, const wchar_t* value)
		{
			return DoSetAttribute(name, wxString(value));
		}
		bool SetAttribute(const wxString& name, int64_t value)
		{
			return DoSetAttribute(name, FormatInt(value));
		}
		bool SetAttribute(const wxString& name, int value)
		{
			return DoSetAttribute(name, FormatInt(value));
		}
		bool SetAttribute(const wxString& name, double value, int precision = -1)
		{
			return DoSetAttribute(name, FormatFloat(value, precision));
		}
		bool SetAttribute(const wxString& name, float value, int precision = -1, bool isCDATA = false)
		{
			return DoSetAttribute(name, FormatFloat((double)value, precision), isCDATA);
		}
		bool SetAttribute(const wxString& name, bool value)
		{
			return DoSetAttribute(name, FormatBool(value));
		}

		/* Navigation */
		virtual Node GetElementByAttribute(const wxString& name, const wxString& value) const = 0;
		virtual Node GetElementByTag(const wxString& tagName) const = 0;

		virtual Node GetParent() const = 0;
		virtual Node GetPreviousSibling() const = 0;
		virtual Node GetNextSibling() const = 0;
		virtual Node GetFirstChild() const = 0;
		virtual Node GetLastChild() const = 0;

		/* Insertion */

		/* Deletion */
};
