/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxXDocumentNode.h"
#include "KxFramework/XML/TinyXML2.h"

enum KxXMLNodeType
{
	KxXML_NODE_INVALID = -1,

	KxXML_NODE_DOCUMENT,
	KxXML_NODE_ELEMENT,
	KxXML_NODE_TEXT,
	KxXML_NODE_COMMENT,
	KxXML_NODE_DECLARATION,
	KxXML_NODE_UNKNOWN,
};
enum KxXMLPrintMode
{
	KxXML_PRINT_NORMAL,
	KxXML_PRINT_HTML5,
};
enum KxXMLInsertNodeMode
{
	KxXML_INSERT_AFTER_CHILD,
	KxXML_INSERT_AS_FIRST_CHILD,
	KxXML_INSERT_AS_LAST_CHILD,
};

class KxXMLPrinter: public tinyxml2::XMLPrinter
{
	public:
		KxXMLPrinter(FILE* file = nullptr, bool compact = false, int depth = 0)
			:XMLPrinter(file, compact, depth)
		{
		}

	protected:
		virtual void PrintSpace(int depth) override
		{
			for (int i = 0; i < depth; i++)
			{
				Write("\t");
			}
		}
};

class KX_API KxXMLNode;
class KX_API KxXMLAttribute
{
	friend class KxXMLNode;
	
	private:
		static const KxXMLAttribute NullAttribute;

	private:
		KxXMLNode* m_Node = nullptr;
		tinyxml2::XMLAttribute* m_Attribute = nullptr;

	private:
		tinyxml2::XMLAttribute* GetAttribute()
		{
			return m_Attribute;
		}

	private:
		KxXMLAttribute() {}
		KxXMLAttribute(tinyxml2::XMLAttribute* attribute, KxXMLNode* node);
		KxXMLAttribute(const tinyxml2::XMLAttribute* attribute, KxXMLNode* node);
		~KxXMLAttribute();

	public:
		bool IsOK() const
		{
			return m_Node != nullptr && m_Attribute != nullptr;
		}

		const KxXMLNode* GetNode() const
		{
			return m_Node;
		}

		wxString GetName() const;
		void SetName(const wxString& name) = delete;
		wxString GetValue() const;
		void SetValue(const wxString& name) = delete;

		KxXMLAttribute Next() const;
};

class KX_API KxXMLDocument;
class KX_API KxXMLNode: public KxXDocumentNode<KxXMLNode>
{
	friend class KxXMLDocument;
	friend class KxXMLAttribute;

	public:
		static const KxXMLNode NullNode;

	protected:
		static wxString ToWxString(const std::string& s)
		{
			return wxString::FromUTF8Unchecked(s.c_str(), s.size());
		}
		static wxString ToWxString(const char* s)
		{
			return wxString::FromUTF8Unchecked(s);
		}
		static std::string CleanText(const tinyxml2::XMLNode* node, const std::string& separator = std::string());
		template<class T = KxXMLPrinter> static wxString PrintDocument(const tinyxml2::XMLDocument& document)
		{
			T buffer;
			document.Print(&buffer);
			return wxString::FromUTF8Unchecked(buffer.CStr(), buffer.CStrSize() - 1);
		}
		static wxString PrintDocument(const tinyxml2::XMLDocument& document, KxXMLPrintMode mode);
		static size_t GetIndexWithinParent(const tinyxml2::XMLNode* node);

	protected:
		virtual const tinyxml2::XMLNode* GetNode() const
		{
			return m_Node;
		}
		virtual tinyxml2::XMLNode* GetNode()
		{
			return m_Node;
		}
		virtual void SetNode(tinyxml2::XMLNode* node)
		{
			m_Node = node;
		}

	private:
		tinyxml2::XMLNode* m_Node = nullptr;
		KxXMLDocument* m_Document = nullptr;

	protected:
		wxString DoGetValue(const wxString& defaultValue = wxEmptyString) const override;
		int64_t DoGetValueIntWithBase(int base = 10, int64_t defaultValue = 0) const override;
		double DoGetValueFloat(double defaultValue = 0.0) const override;
		bool DoGetValueBool(bool defaultValue = false) const override;
		bool DoSetValue(const wxString& value, bool isCDATA = false) override;

		wxString DoGetAttribute(const wxString& name, const wxString& defaultValue = wxEmptyString) const override;
		int64_t DoGetAttributeIntWithBase(const wxString& name, int base, int64_t defaultValue = 0) const override;
		double DoGetAttributeFloat(const wxString& name, double defaultValue = 0.0) const override;
		bool DoGetAttributeBool(const wxString& name, bool defaultValue = false) const override;
		bool DoSetAttribute(const wxString& name, const wxString& value) override;

	protected:
		KxXMLNode(tinyxml2::XMLNode* node, KxXMLDocument* document);
		KxXMLNode(const tinyxml2::XMLNode* node, KxXMLDocument* document);

	public:
		KxXMLNode() = default;
		KxXMLNode(const KxXMLNode&) = default;

	public:
		/* General */
		bool IsOK() const override
		{
			return m_Node && m_Document;
		}
		wxString GetXPath() const override
		{
			return ConstructXPath(*this);
		}
		KxXMLNode QueryElement(const wxString& XPath) const override;
		KxXMLNode QueryOrCreateElement(const wxString& XPath) override;

		wxString GetXPathIndexSeparator() const override;
		bool SetXPathIndexSeparator(const wxString& value) override;

		/* Node */
		size_t GetIndexWithinParent() const override;
		wxString GetName() const override;
		bool SetName(const wxString& name) override;

		size_t GetChildrenCount() const override;
		bool HasChildren() const override;
		NodeVector GetChildren() const override;
		NodeVector GetChildrenElements(const wxString& searchPattern = wxEmptyString) const;
		bool ClearChildren() override;
		bool ClearNode() override;

		KxXMLDocument* GetDocumentNode()
		{
			return m_Document;
		}
		const KxXMLDocument* GetDocumentNode() const
		{
			return m_Document;
		}

		virtual wxString GetXML(KxXMLPrintMode mode = KxXML_PRINT_NORMAL) const;
		KxXMLNodeType GetType() const;
		bool IsElement() const;
		bool IsText() const;
		
		/* Value */
		wxString GetValueText(const wxString& separator = wxEmptyString) const;
		
		bool IsCDATA() const;
		bool SetCDATA(bool value);

		/* Attributes */
		size_t GetAttributeCount() const override;
		bool HasAttributes() const override;
		KxStringVector GetAttributes() const override;

		bool HasAttribute(const wxString& name) const override;
		bool RemoveAttribute(const wxString& name) override;
		bool RemoveAttribute(KxXMLAttribute& attribute);
		bool ClearAttributes() override;
		
		/* Navigation */
		KxXMLNode GetElementByAttribute(const wxString& name, const wxString& value) const override;
		KxXMLNode GetElementByTag(const wxString& tagName) const override;
		KxXMLNode GetParent() const override;
		KxXMLNode GetPreviousSibling() const override;
		KxXMLNode GetPreviousSiblingElement(const wxString& name = wxEmptyString) const;
		KxXMLNode GetNextSibling() const override;
		KxXMLNode GetNextSiblingElement(const wxString& name = wxEmptyString) const;
		KxXMLNode GetFirstChild() const override;
		KxXMLNode GetFirstChildElement(const wxString& name = wxEmptyString) const;
		KxXMLNode GetLastChild() const override;
		KxXMLNode GetLastChildElement(const wxString& name = wxEmptyString) const;

		/* Insertion */
		bool InsertAfterChild(KxXMLNode& newNode);
		bool InsertFirstChild(KxXMLNode& newNode);
		bool InsertLastChild(KxXMLNode& newNode);
		bool Insert(KxXMLNode& node, KxXMLInsertNodeMode insertMode);

		KxXMLNode NewElement(const wxString& name, KxXMLInsertNodeMode insertMode = KxXML_INSERT_AS_LAST_CHILD);
		KxXMLNode NewComment(const wxString& value, KxXMLInsertNodeMode insertMode = KxXML_INSERT_AS_LAST_CHILD);
		KxXMLNode NewText(const wxString& value, KxXMLInsertNodeMode insertMode = KxXML_INSERT_AS_LAST_CHILD);
		KxXMLNode NewDeclaration(const wxString& value, KxXMLInsertNodeMode insertMode = KxXML_INSERT_AS_LAST_CHILD);
		KxXMLNode NewUnknown(const wxString& value, KxXMLInsertNodeMode insertMode = KxXML_INSERT_AS_LAST_CHILD);
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxXMLDocument: public KxXMLNode
{
	friend class KxXMLNode;

	private:
		tinyxml2::XMLDocument m_Document;
		wxString m_XPathDelimiter;
		wxString m_DeclaredEncoding;

	private:
		tinyxml2::XMLDocument* GetDocument()
		{
			return &m_Document;
		}
		int ExtractIndexFromName(wxString& elementName) const;
		void ReplaceDeclaration();
		
		void Init();
		bool Load(const char* xmlText, size_t length);
		void UnLoad();

		const tinyxml2::XMLNode* GetNode() const override
		{
			return &m_Document;
		}
		tinyxml2::XMLNode* GetNode() override
		{
			return &m_Document;
		}
		void SetNode(tinyxml2::XMLNode*) override
		{
		}

	private:
		KxXMLNode CreateElement(const wxString& name);
		KxXMLNode CreateComment(const wxString& value);
		KxXMLNode CreateText(const wxString& value);
		KxXMLNode CreateDeclaration(const wxString& value);
		KxXMLNode CreateUnknown(const wxString& value);

	public:
		KxXMLDocument(const wxString& xmlText = wxEmptyString);
		KxXMLDocument(wxInputStream& stream);
		virtual ~KxXMLDocument();

	public:
		bool IsOK() const override;
		wxString GetXPath() const override;
		wxString GetXML(KxXMLPrintMode mode = KxXML_PRINT_NORMAL) const override;

		bool Load(const wxString& xmlText);
		bool Load(wxInputStream& stream);
		bool Save(wxOutputStream& stream) const;
		wxString Save() const;

		const wxString& GetDeclaredEncoding() const
		{
			return m_DeclaredEncoding;
		}
		void SetDeclaredEncoding(const wxString& value)
		{
			m_DeclaredEncoding = value;
		}

		wxString GetXPathIndexSeparator() const override
		{
			return m_XPathDelimiter;
		}
		bool SetXPathIndexSeparator(const wxString& value) override
		{
			m_XPathDelimiter = value;
			return !m_XPathDelimiter.IsEmpty();
		}

		/* Deletion */
		bool RemoveNode(KxXMLNode& node);
};
