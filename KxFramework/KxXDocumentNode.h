/*
Copyright © 2019 Kerber. All rights reserved.

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
	
	private:
		template<class T> using RemoveRCV = typename std::remove_cv_t<std::remove_reference_t<T>>;

		template<class T> constexpr static bool TestIntType()
		{
			return std::is_integral_v<RemoveRCV<T>> || std::is_enum_v<RemoveRCV<T>>;
		}
		template<class T> constexpr static bool TestFloatType()
		{
			return std::is_floating_point_v<RemoveRCV<T>>;
		}
		template<class T> constexpr static bool TestPtrType()
		{
			return std::is_pointer_v<RemoveRCV<T>>;
		}

		template<class T> constexpr static void AssertIntType()
		{
			static_assert(TestIntType<T>(), "T must be of integral or enum type");
		}
		template<class T> constexpr static void AssertFloatType()
		{
			static_assert(TestFloatType<T>(), "T must be floating point number");
		}
		template<class T> constexpr static void AssertPtrType()
		{
			static_assert(TestPtrType<T>(), "T must be a pointer");
		}

	protected:
		virtual wxString FormatInt(int64_t value, int base = 10) const;
		virtual wxString FormatPointer(const void* value) const;
		virtual wxString FormatFloat(double value, int precision = -1) const;
		virtual wxString FormatBool(bool value) const;

		virtual int64_t ParseInt(const wxString& value, int base = 10, int64_t defaultValue = 0) const;
		virtual void* ParsePointer(const wxString& value, void* defaultValue = nullptr) const;
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
		bool GetValueBool(bool defaultValue = false) const
		{
			return DoGetValueBool(defaultValue);
		}
		double GetValueFloat(double defaultValue = 0.0) const
		{
			return DoGetValueFloat(defaultValue);
		}

		template<class T = void, class TDefault = T>
		T* GetValuePtr(TDefault* defaultValue = nullptr) const
		{
			AssertPtrType<T>();

			return reinterpret_cast<T*>(DoGetValueIntWithBase(16, reinterpret_cast<int64_t>(defaultValue)));
		}

		template<class T = int64_t, class TDefault = T>
		T GetValueInt(TDefault defaultValue = 0) const
		{
			AssertIntType<T>();

			return static_cast<T>(DoGetValueIntWithBase(10, static_cast<int64_t>(defaultValue)));
		}
		
		template<class T = int64_t, class TDefault = T>
		T GetValueIntWithBase(int base, TDefault defaultValue = 0) const
		{
			AssertIntType<T>();

			return static_cast<T>(DoGetValueIntWithBase(base, static_cast<int64_t>(defaultValue)));
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
		bool SetValue(bool value)
		{
			return DoSetValue(FormatBool(value), AsCDATA::Never);
		}
		bool SetValue(float value, int precision = -1)
		{
			return DoSetValue(FormatFloat(static_cast<double>(value), precision), AsCDATA::Never);
		}
		bool SetValue(double value, int precision = -1)
		{
			return DoSetValue(FormatFloat(value, precision), AsCDATA::Never);
		}
		bool SetValue(const void* value)
		{
			return DoSetValue(FormatPointer(value));
		}
		bool SetValue(std::nullptr_t)
		{
			return DoSetValue(FormatPointer(nullptr));
		}

		template<class T>
		std::enable_if_t<TestIntType<T>(), bool> SetValue(T value, int base = 10)
		{
			return DoSetValue(FormatInt(static_cast<int64_t>(value), base), AsCDATA::Never);
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
		bool GetAttributeBool(const wxString& name, bool defaultValue = false) const
		{
			return DoGetAttributeBool(name, defaultValue);
		}
		double GetAttributeFloat(const wxString& name, double defaultValue = 0.0) const
		{
			return DoGetAttributeFloat(name, defaultValue);
		}

		template<class T = void, class TDefault = T>
		T* GetAttributePtr(const wxString& name, TDefault* defaultValue = nullptr) const
		{
			AssertPtrType<T>();

			return reinterpret_cast<T*>(DoGetAttributeIntWithBase(name, 16, reinterpret_cast<int64_t>(defaultValue)));
		}
		
		template<class T = int64_t, class TDefault = T>
		T GetAttributeInt(const wxString& name, TDefault defaultValue = 0) const
		{
			AssertIntType<T>();

			return static_cast<T>(DoGetAttributeIntWithBase(name, 10, static_cast<int64_t>(defaultValue)));
		}

		template<class T = int64_t, class TDefault = T>
		T GetAttributeIntWithBase(const wxString& name, int base, TDefault defaultValue = 0) const
		{
			AssertIntType<T>();

			return static_cast<T>(DoGetAttributeIntWithBase(name, base, static_cast<int64_t>(defaultValue)));
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
		bool SetAttribute(const wxString& name, bool value)
		{
			return DoSetAttribute(name, FormatBool(value));
		}
		bool SetAttribute(const wxString& name, float value, int precision = -1)
		{
			return DoSetAttribute(name, FormatFloat(static_cast<double>(value), precision));
		}
		bool SetAttribute(const wxString& name, double value, int precision = -1)
		{
			return DoSetAttribute(name, FormatFloat(value, precision));
		}
		bool SetAttribute(const wxString& name, const void* value)
		{
			return DoSetAttribute(name, FormatPointer(value));
		}
		bool SetAttribute(const wxString& name, std::nullptr_t)
		{
			return DoSetAttribute(name, FormatPointer(nullptr));
		}

		template<class T>
		std::enable_if_t<TestIntType<T>(), bool> SetAttribute(const wxString& name, T value, int base = 10)
		{
			return DoSetAttribute(name, FormatInt(static_cast<int64_t>(value), base));
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
