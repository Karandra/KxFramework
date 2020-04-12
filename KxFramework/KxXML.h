#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxXDocumentNode.h"
#include "TinyXML2.h"
class KX_API KxXMLNode;
class KX_API KxXMLDocument;
class KX_API KxXMLAttribute;

enum class KxXMLNodeType
{
	Invalid = -1,

	Document,
	Element,
	Text,
	Comment,
	Declaration,
	Unknown,
};
enum class KxXMLPrintMode
{
	Default,
	HTML5,
};
enum class KxXMLInsertNode
{
	AfterChild,
	AsFirstChild,
	AsLastChild,
};

class KX_API KxXMLAttribute final
{
	friend class KxXMLNode;

	private:
		KxXMLNode* m_Node = nullptr;
		tinyxml2::XMLAttribute* m_Attribute = nullptr;

	private:
		tinyxml2::XMLAttribute* GetAttribute()
		{
			return m_Attribute;
		}

	private:
		KxXMLAttribute() = default;
		KxXMLAttribute(KxXMLNode& node, tinyxml2::XMLAttribute& attribute)
			:m_Node(&node), m_Attribute(&attribute)
		{
		}
		KxXMLAttribute(const KxXMLNode& node, const tinyxml2::XMLAttribute& attribute)
			:m_Node(const_cast<KxXMLNode*>(&node)), m_Attribute(const_cast<tinyxml2::XMLAttribute*>(&attribute))
		{
		}

	public:
		bool IsOK() const
		{
			return m_Node && m_Attribute;
		}
		KxXMLNode GetNode() const;
		KxXMLDocument* GetDocument() const;

		wxString GetName() const;
		void SetName(const wxString& name) = delete;

		wxString GetValue() const;
		void SetValue(const wxString& name) = delete;

		KxXMLAttribute Next() const;

	public:
		explicit operator bool() const
		{
			return IsOK();
		}
		bool operator!() const
		{
			return !IsOK();
		}
};

class KX_API KxXMLNode: public KxXDocumentNode<KxXMLNode>
{
	friend class KxXMLDocument;
	friend class KxXMLAttribute;

	private:
		KxXMLDocument* m_Document = nullptr;
		tinyxml2::XMLNode* m_Node = nullptr;

	private:
		KxXMLNode ConstructOrQueryElement(const wxString& xPath, bool allowCreate);

	protected:
		const tinyxml2::XMLNode* GetNode() const
		{
			return m_Node;
		}
		tinyxml2::XMLNode* GetNode()
		{
			return m_Node;
		}

	protected:
		wxString DoGetValue(const wxString& defaultValue = wxEmptyString) const override;
		int64_t DoGetValueIntWithBase(int base, int64_t defaultValue = 0) const override;
		double DoGetValueFloat(double defaultValue = 0.0) const override;
		bool DoGetValueBool(bool defaultValue = false) const override;
		bool DoSetValue(const wxString& value, WriteEmpty writeEmpty, AsCDATA asCDATA) override;

		wxString DoGetAttribute(const wxString& name, const wxString& defaultValue = wxEmptyString) const override;
		int64_t DoGetAttributeIntWithBase(const wxString& name, int base, int64_t defaultValue = 0) const override;
		double DoGetAttributeFloat(const wxString& name, double defaultValue = 0.0) const override;
		bool DoGetAttributeBool(const wxString& name, bool defaultValue = false) const override;
		bool DoSetAttribute(const wxString& name, const wxString& value, WriteEmpty writeEmpty) override;

	protected:
		KxXMLNode(tinyxml2::XMLNode* node, KxXMLDocument& document)
			:m_Node(node), m_Document(&document)
		{
		}
		KxXMLNode(const tinyxml2::XMLNode* node, KxXMLDocument& document)
			:m_Node(const_cast<tinyxml2::XMLNode*>(node)), m_Document(&document)
		{
		}

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
		KxXMLNode QueryElement(const wxString& xPath) const override;
		KxXMLNode ConstructElement(const wxString& xPath) override;

		wxString GetXPathIndexSeparator() const override;
		void SetXPathIndexSeparator(const wxString& value) override;

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

		KxXMLDocument& GetDocument()
		{
			return *m_Document;
		}
		const KxXMLDocument& GetDocument() const
		{
			return *m_Document;
		}

		virtual wxString GetXML(KxXMLPrintMode mode = KxXMLPrintMode::Default) const;
		KxXMLNodeType GetType() const;
		bool IsElement() const;
		bool IsText() const;
		
		/* Value */
		wxString GetValueText(const wxString& separator = wxEmptyString) const;
		
		bool IsCDATA() const override;
		bool SetCDATA(bool value) override;

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
		bool Insert(KxXMLNode& node, KxXMLInsertNode insertMode);

		KxXMLNode NewElement(const wxString& name, KxXMLInsertNode insertMode = KxXMLInsertNode::AsLastChild);
		KxXMLNode NewComment(const wxString& value, KxXMLInsertNode insertMode = KxXMLInsertNode::AsLastChild);
		KxXMLNode NewText(const wxString& value, KxXMLInsertNode insertMode = KxXMLInsertNode::AsLastChild);
		KxXMLNode NewDeclaration(const wxString& value, KxXMLInsertNode insertMode = KxXMLInsertNode::AsLastChild);
		KxXMLNode NewUnknown(const wxString& value, KxXMLInsertNode insertMode = KxXMLInsertNode::AsLastChild);
};

class KX_API KxXMLDocument: public KxXMLNode
{
	friend class KxXMLNode;

	public:
		static wxString GetLibraryName();
		static wxString GetLibraryVersion();

	private:
		tinyxml2::XMLDocument m_Document;
		wxString m_XPathIndexSeparator;
		wxString m_DeclaredEncoding;

	private:
		tinyxml2::XMLDocument* GetDocument()
		{
			return &m_Document;
		}
		void ReplaceDeclaration();
		
		void Init();
		bool DoLoad(const char* xml, size_t length);
		void DoUnload();

	private:
		KxXMLNode CreateElement(const wxString& name);
		KxXMLNode CreateComment(const wxString& value);
		KxXMLNode CreateText(const wxString& value);
		KxXMLNode CreateDeclaration(const wxString& value);
		KxXMLNode CreateUnknown(const wxString& value);

	public:
		KxXMLDocument()
			:KxXMLNode(&m_Document, *this)
		{
			Init();
		}
		KxXMLDocument(const wxString& xml)
			:KxXMLDocument()
		{
			Load(xml);
		}
		KxXMLDocument(std::string_view xml)
			:KxXMLDocument()
		{
			Load(xml);
		}
		KxXMLDocument(std::wstring_view xml)
			:KxXMLDocument()
		{
			Load(xml);
		}
		KxXMLDocument(const char* xml, size_t length = wxString::npos)
			:KxXMLDocument()
		{
			Load(std::string_view(xml, length));
		}
		KxXMLDocument(const wchar_t* xml, size_t length = wxString::npos)
			:KxXMLDocument()
		{
			Load(std::wstring_view(xml, length));
		}
		KxXMLDocument(wxInputStream& stream)
			:KxXMLDocument()
		{
			Load(stream);
		}
		KxXMLDocument(const KxXMLDocument& other)
			:KxXMLDocument()
		{
			*this = other;
		}
		~KxXMLDocument()
		{
			DoUnload();
		}

	public:
		bool IsOK() const override;
		wxString GetXPath() const override;
		wxString GetXML(KxXMLPrintMode mode = KxXMLPrintMode::Default) const override;

		bool Load(const wxString& xml);
		bool Load(const char* xml, size_t length = wxString::npos)
		{
			return Load(std::string_view(xml, length));
		}
		bool Load(const wchar_t* xml, size_t length = wxString::npos)
		{
			return Load(std::wstring_view(xml, length));
		}
		bool Load(std::string_view xml);
		bool Load(std::wstring_view xml);
		bool Load(wxInputStream& stream);
		bool Save(wxOutputStream& stream) const;
		wxString Save() const;

		wxString GetDeclaredEncoding() const
		{
			return m_DeclaredEncoding;
		}
		void SetDeclaredEncoding(const wxString& value)
		{
			m_DeclaredEncoding = value;
		}

		wxString GetXPathIndexSeparator() const override
		{
			return m_XPathIndexSeparator;
		}
		void SetXPathIndexSeparator(const wxString& value) override
		{
			if (value.IsEmpty())
			{
				m_XPathIndexSeparator = KxIXDocumentNode::GetXPathIndexSeparator();
			}
			else
			{
				m_XPathIndexSeparator = value;
			}
		}

		/* Deletion */
		bool RemoveNode(KxXMLNode& node);

	public:
		KxXMLDocument& operator=(const KxXMLDocument& other);
};
