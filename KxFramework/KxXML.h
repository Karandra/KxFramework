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
		KxXMLPrinter(FILE* file = NULL, bool compact = false, int depth = 0)
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

class KxXMLNode;
class KxXMLAttribute
{
	friend class KxXMLNode;
	
	private:
		static const KxXMLAttribute NullAttribute;

	private:
		KxXMLNode* m_Node = NULL;
		tinyxml2::XMLAttribute* m_Attribute = NULL;

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
			return m_Node != NULL && m_Attribute != NULL;
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

class KxXMLDocument;
class KxXMLNode: public KxXDocumentNode<KxXMLNode>
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
		tinyxml2::XMLNode* m_Node = NULL;
		KxXMLDocument* m_Document = NULL;

	protected:
		virtual bool DoSetValue(const wxString& value, bool isCDATA = false) override;
		virtual bool DoSetAttribute(const wxString& name, const wxString& value) override;

	public:
		KxXMLNode()
			:m_Document(NULL), m_Node(NULL)
		{
		}
		virtual ~KxXMLNode();

	protected:
		KxXMLNode(tinyxml2::XMLNode* node, KxXMLDocument* document);
		KxXMLNode(const tinyxml2::XMLNode* node, KxXMLDocument* document);

	public:
		/* General */
		virtual bool IsOK() const override
		{
			return m_Node && m_Document;
		}
		virtual KxXMLNode QueryElement(const wxString& XPath) const override;

		/* Node */
		virtual size_t GetIndexWithinParent() const override;
		virtual wxString GetName() const override;
		virtual bool SetName(const wxString& name) override;

		virtual size_t GetChildrenCount() const override;
		virtual bool HasChildren() const override;
		virtual NodeVector GetChildren() const override;
		NodeVector GetChildrenElements(const wxString& searchPattern = wxEmptyString) const;
		KxStringVector GetElementNames(bool bFullNames = false, bool includeIndexes = false);
		virtual bool ClearChildren() override;
		virtual bool ClearNode() override;

		KxXMLDocument* GetDocumentNode()
		{
			return m_Document;
		}
		const KxXMLDocument* GetDocumentNode() const
		{
			return m_Document;
		}

		virtual wxString GetXML(KxXMLPrintMode mode = KxXML_PRINT_NORMAL) const;
		wxString GetXPath(bool includeIndexes = true) const;
		KxXMLNodeType GetType() const;
		bool IsElement() const;
		bool IsText() const;
		
		/* Value */
		wxString GetValueText(const wxString& separator = wxEmptyString) const;
		virtual wxString GetValue(const wxString& defaultValue = wxEmptyString) const;
		virtual int64_t GetValueInt(int64_t defaultValue = 0) const override;
		virtual double GetValueFloat(double defaultValue = 0.0) const;
		virtual bool GetValueBool(bool defaultValue = false) const;
		
		virtual bool IsCDATA() const;
		virtual bool SetCDATA(bool value);

		/* Attributes */
		virtual size_t GetAttributeCount() const override;
		virtual bool HasAttributes() const override;
		virtual KxStringVector GetAttributes() const override;

		virtual bool HasAttribute(const wxString& name) const override;
		virtual bool RemoveAttribute(const wxString& name) override;
		bool RemoveAttribute(KxXMLAttribute& attribute);
		virtual bool ClearAttributes() override;

		virtual wxString GetAttribute(const wxString& name, const wxString& defaultValue = wxEmptyString) const override;
		virtual int64_t GetAttributeInt(const wxString& name, int64_t defaultValue = 0) const override;
		virtual double GetAttributeFloat(const wxString& name, double defaultValue = 0.0) const override;
		virtual bool GetAttributeBool(const wxString& name, bool defaultValue = false) const override;
		
		/* Navigation */
		virtual KxXMLNode GetElementByAttribute(const wxString& name, const wxString& value) const override;
		virtual KxXMLNode GetElementByTag(const wxString& tagName) const override;
		virtual KxXMLNode GetParent() const override;
		virtual KxXMLNode GetPreviousSibling() const override;
		KxXMLNode GetPreviousSiblingElement(const wxString& name = wxEmptyString) const;
		virtual KxXMLNode GetNextSibling() const override;
		KxXMLNode GetNextSiblingElement(const wxString& name = wxEmptyString) const;
		virtual KxXMLNode GetFirstChild() const override;
		KxXMLNode GetFirstChildElement(const wxString& name = wxEmptyString) const;
		virtual KxXMLNode GetLastChild() const override;
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

class KxXMLDocument: public KxXMLNode
{
	friend class KxXMLNode;

	private:
		tinyxml2::XMLDocument m_Document;
		wxString m_XPathDelimiter = DefaultPathDelimiter;
		wxString m_DeclaredEncoding = DefaultDeclaredEncoding;

	private:
		tinyxml2::XMLDocument* GetDocument()
		{
			return &m_Document;
		}
		int ExtractIndexFromName(wxString& elementName) const;
		void ReplaceDeclaration();
		
		bool Load(const char* xmlText, size_t length);
		void UnLoad();

		virtual const tinyxml2::XMLNode* GetNode() const override
		{
			return &m_Document;
		}
		virtual tinyxml2::XMLNode* GetNode() override
		{
			return &m_Document;
		}
		virtual void SetNode(tinyxml2::XMLNode*) override
		{
		}

	private:
		KxXMLNode CreateElement(const wxString& name);
		KxXMLNode CreateComment(const wxString& value);
		KxXMLNode CreateText(const wxString& value);
		KxXMLNode CreateDeclaration(const wxString& value);
		KxXMLNode CreateUnknown(const wxString& value);

	public:
		static const wxString DefaultPathDelimiter;
		static const wxString DefaultDeclaredEncoding;

		KxXMLDocument(const wxString& xmlText = wxEmptyString);
		KxXMLDocument(wxInputStream& stream);
		virtual ~KxXMLDocument();

	public:
		virtual bool IsOK() const override;
		virtual wxString GetXML(KxXMLPrintMode mode = KxXML_PRINT_NORMAL) const override;

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

		bool IsXPathSeparatorUsed() const
		{
			return !m_XPathDelimiter.IsEmpty();
		}
		const wxString& GetXPathSeparator() const
		{
			return m_XPathDelimiter;
		}
		void SetXPathSeparator(const wxString& value = wxEmptyString)
		{
			m_XPathDelimiter = value;
		}

		/* Deletion */
		bool RemoveNode(KxXMLNode& node);
};
