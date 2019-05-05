/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxIXDocumentNode
{
	public:
		enum class AsCDATA
		{
			Auto = -1,
			Always = 1,
			Never = 0,
		};

	public:
		static int ExtractIndexFromName(wxString& elementName, const wxString& xPathSeparator);
		static bool ContainsForbiddenCharactersForValue(const wxString& value);
		template<class TNode> static wxString ConstructXPath(const TNode& thisNode)
		{
			wxString xPath;
			const wxString xPathSep = thisNode.GetXPathSeparator();
			const wxString xPathIndexSep = thisNode.GetXPathIndexSeparator();
			bool isFirst = true;

			for (TNode node = thisNode; node.IsOK(); node = node.GetParent())
			{
				const size_t index = node.GetIndexWithinParent();
				if (index > 1)
				{
					xPath.Prepend(wxString::Format(wxS("%s%s%zu"), node.GetName(), xPathIndexSep, index));
				}
				else
				{
					xPath.Prepend(node.GetName());
				}

				if (!isFirst)
				{
					xPath.Prepend(xPathSep);
				}
				isFirst = false;
			}
			return xPath;
		}

	protected:
		virtual wxString FormatInt(int64_t value, int base = 10) const;
		virtual wxString FormatFloat(double value, int precision = -1) const;
		virtual wxString FormatBool(bool value) const;

		virtual int64_t ParseInt(const wxString& value, int base = 10, int64_t defaultValue = 0) const;
		virtual double ParseFloat(const wxString& value, double defaultValue = 0.0) const;
		virtual bool ParseBool(const wxString& value, bool defaultValue = false) const;

	protected:
		virtual wxString DoGetValue(const wxString& defaultValue = wxEmptyString) const = 0;
		virtual int64_t DoGetValueIntWithBase(int base, int64_t defaultValue = 0) const
		{
			return ParseInt(DoGetValue(), base, defaultValue);
		}
		virtual double DoGetValueFloat(double defaultValue = 0.0) const
		{
			return ParseFloat(DoGetValue(), defaultValue);
		}
		virtual bool DoGetValueBool(bool defaultValue = false) const
		{
			return ParseBool(DoGetValue(), defaultValue);
		}
		virtual bool DoSetValue(const wxString& value, AsCDATA asCDATA = AsCDATA::Auto) = 0;

		virtual wxString DoGetAttribute(const wxString& name, const wxString& defaultValue = wxEmptyString) const = 0;
		virtual int64_t DoGetAttributeIntWithBase(const wxString& name, int base, int64_t defaultValue = 0) const
		{
			return ParseInt(DoGetAttribute(name), base, defaultValue);
		}
		virtual double DoGetAttributeFloat(const wxString& name, double defaultValue = 0.0) const
		{
			return ParseFloat(DoGetAttribute(name), defaultValue);
		}
		virtual bool DoGetAttributeBool(const wxString& name, bool defaultValue = false) const
		{
			return ParseBool(DoGetAttribute(name), defaultValue);
		}
		virtual bool DoSetAttribute(const wxString& name, const wxString& value) = 0;

	public:
		virtual ~KxIXDocumentNode() = default;

	public:
		/* General */
		virtual bool IsOK() const = 0;
		virtual wxString GetXPath() const
		{
			return wxEmptyString;
		}

		wxString GetXPathSeparator() const
		{
			return wxS('/');
		}
		int ExtractIndexFromName(wxString& elementName) const
		{
			return ExtractIndexFromName(elementName, GetXPathIndexSeparator());
		}

		virtual wxString GetXPathIndexSeparator() const
		{
			return wxS(':');
		}
		virtual bool SetXPathIndexSeparator(const wxString& value)
		{
			return false;
		}

		/* Node */
		virtual size_t GetIndexWithinParent() const
		{
			return 0;
		}
		virtual wxString GetName() const
		{
			return wxEmptyString;
		}
		virtual bool SetName(const wxString& name)
		{
			return false;
		}

		virtual size_t GetChildrenCount() const
		{
			return 0;
		}
		virtual bool HasChildren() const
		{
			return GetChildrenCount() != 0;
		}
		virtual bool ClearChildren()
		{
			return false;
		}
		virtual bool ClearNode()
		{
			return false;
		}

		/* Values */
		wxString GetValue(const wxString& defaultValue = wxEmptyString) const
		{
			return DoGetValue(defaultValue);
		}
		int64_t GetValueInt(int64_t defaultValue = 0) const
		{
			return DoGetValueIntWithBase(10, defaultValue);
		}
		int64_t GetValueIntWithBase(int base, int64_t defaultValue = 0) const
		{
			return DoGetValueIntWithBase(base, defaultValue);
		}
		double GetValueFloat(double defaultValue = 0.0) const
		{
			return DoGetValueFloat(defaultValue);
		}
		bool GetValueBool(bool defaultValue = false) const
		{
			return DoGetValueBool(defaultValue);
		}

		bool SetValue(const char* value, AsCDATA asCDATA = AsCDATA::Auto)
		{
			return DoSetValue(wxString::FromUTF8(value), asCDATA);
		}
		bool SetValue(const wchar_t* value, AsCDATA asCDATA = AsCDATA::Auto)
		{
			return DoSetValue(value, asCDATA);
		}
		bool SetValue(const wxString& value, AsCDATA asCDATA = AsCDATA::Auto)
		{
			return DoSetValue(value, asCDATA);
		}
		bool SetValue(int value, int base = 10)
		{
			return DoSetValue(FormatInt(value, base), AsCDATA::Never);
		}
		bool SetValue(int64_t value, int base = 10)
		{
			return DoSetValue(FormatInt(value, base), AsCDATA::Never);
		}
		bool SetValue(double value, int precision = -1)
		{
			return DoSetValue(FormatFloat(value, precision), AsCDATA::Never);
		}
		bool SetValue(float value, int precision = -1)
		{
			return DoSetValue(FormatFloat((double)value, precision), AsCDATA::Never);
		}
		bool SetValue(bool value)
		{
			return DoSetValue(FormatBool(value), AsCDATA::Never);
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
		virtual size_t GetAttributeCount() const
		{
			return 0;
		}
		virtual bool HasAttributes() const
		{
			return GetAttributeCount() != 0;
		}
		virtual KxStringVector GetAttributes() const
		{
			return {};
		}

		virtual bool HasAttribute(const wxString& name) const
		{
			return false;
		}
		virtual bool RemoveAttribute(const wxString& name)
		{
			return false;
		}
		virtual bool ClearAttributes()
		{
			return false;
		}

		wxString GetAttribute(const wxString& name, const wxString& defaultValue = wxEmptyString) const
		{
			return DoGetAttribute(name, defaultValue);
		}
		int64_t GetAttributeInt(const wxString& name, int64_t defaultValue = 0) const
		{
			return DoGetAttributeIntWithBase(name, 10, defaultValue);
		}
		int64_t GetAttributeIntWithBase(const wxString& name, int base, int64_t defaultValue = 0) const
		{
			return DoGetAttributeIntWithBase(name, base, defaultValue);
		}
		double GetAttributeFloat(const wxString& name, double defaultValue = 0.0) const
		{
			return DoGetAttributeFloat(name, defaultValue);
		}
		bool GetAttributeBool(const wxString& name, bool defaultValue = false) const
		{
			return DoGetAttributeBool(name, defaultValue);
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
		bool SetAttribute(const wxString& name, int64_t value, int base = 10)
		{
			return DoSetAttribute(name, FormatInt(value, base));
		}
		bool SetAttribute(const wxString& name, int value, int base = 10)
		{
			return DoSetAttribute(name, FormatInt(value, base));
		}
		bool SetAttribute(const wxString& name, double value, int precision = -1)
		{
			return DoSetAttribute(name, FormatFloat(value, precision));
		}
		bool SetAttribute(const wxString& name, float value, int precision = -1)
		{
			return DoSetAttribute(name, FormatFloat((double)value, precision));
		}
		bool SetAttribute(const wxString& name, bool value)
		{
			return DoSetAttribute(name, FormatBool(value));
		}
};

template<class NodeT>
class KxXDocumentNode: public KxIXDocumentNode
{
	public:
		using Node = NodeT;
		using NodeVector = std::vector<Node>;

	public:
		/* General */
		virtual Node QueryElement(const wxString& XPath) const
		{
			return {};
		}
		virtual Node QueryOrCreateElement(const wxString& XPath)
		{
			return {};
		}

		/* Node */
		virtual NodeVector GetChildren() const
		{
			return {};
		}

		/* Navigation */
		virtual Node GetElementByAttribute(const wxString& name, const wxString& value) const
		{
			return {};
		}
		virtual Node GetElementByTag(const wxString& tagName) const
		{
			return {};
		}

		virtual Node GetParent() const
		{
			return {};
		}
		virtual Node GetPreviousSibling() const
		{
			return {};
		}
		virtual Node GetNextSibling() const
		{
			return {};
		}
		virtual Node GetFirstChild() const
		{
			return {};
		}
		virtual Node GetLastChild() const
		{
			return {};
		}

		/* Insertion */

		/* Deletion */
};
