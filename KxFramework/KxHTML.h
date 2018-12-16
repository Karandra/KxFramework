/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxXDocumentNode.h"

enum KxHTML_NodeType;
enum KxHTML_TagType;

class KxHTMLDocument;
class KX_API KxHTMLNode: public KxXDocumentNode<KxHTMLNode>
{
	friend class KxHTMLDocument;

	public:
		static const KxHTMLNode NullNode;

	private:
		static wxString ToWxString(const std::string& s)
		{
			return wxString::FromUTF8Unchecked(s.c_str(), s.size());
		}

		virtual const void* GetNode() const
		{
			return m_Node;
		}
		virtual void SetNode(const void* node)
		{
			m_Node = node;
		}

	private:
		const void* m_Node = NULL;
		const KxHTMLDocument* m_Document = NULL;

	protected:
		virtual bool DoSetValue(const wxString& value, bool isCDATA = false) override
		{
			return false;
		}
		virtual bool DoSetAttribute(const wxString& name, const wxString& value) override
		{
			return false;
		}

	public:
		KxHTMLNode()
			:m_Document(NULL), m_Node(NULL)
		{
		}
		KxHTMLNode(const KxHTMLNode&) = default;
		virtual ~KxHTMLNode()
		{
		}

	protected:
		KxHTMLNode(const void* node, const KxHTMLDocument* document)
			:m_Node(node), m_Document(document)
		{
		}

	public:
		/* General */
		virtual bool IsOK() const override
		{
			return m_Node && m_Document;
		}
		virtual KxHTMLNode QueryElement(const wxString& XPath) const override;
		virtual KxHTMLNode QueryOrCreateElement(const wxString& XPath) override;

		/* Node */
		virtual size_t GetIndexWithinParent() const override;
		virtual wxString GetName() const override;

		virtual size_t GetChildrenCount() const override;
		virtual NodeVector GetChildren() const override;

		virtual wxString GetHTML() const;
		KxHTML_NodeType GetType() const;
		KxHTML_TagType GetTagType() const;

		const KxHTMLDocument* GetDocumentNode() const
		{
			return m_Document;
		}
		bool IsFullNode() const;

		/* Value */
		wxString GetValueText() const;
		virtual wxString GetValue(const wxString& defaultValue = wxEmptyString) const override;
		
		/* Attributes */
		virtual size_t GetAttributeCount() const override;
		virtual KxStringVector GetAttributes() const override;

		virtual bool HasAttribute(const wxString& name) const override;
		virtual wxString GetAttribute(const wxString& name, const wxString& defaultValue = wxEmptyString) const override;
		
		/* Navigation */
		virtual KxHTMLNode GetElementByAttribute(const wxString& name, const wxString& value) const override;
		KxHTMLNode GetElementByID(const wxString& id) const
		{
			return GetElementByAttribute("id", id);
		}
		KxHTMLNode GetElementByClass(const wxString & className) const
		{
			return GetElementByAttribute("class", className);
		}
		KxHTMLNode GetElementByTag(KxHTML_TagType tagType) const;
		KxHTMLNode GetElementByTag(const wxString& tagName) const;
		
		virtual KxHTMLNode GetParent() const override;
		virtual KxHTMLNode GetPreviousSibling() const override;
		virtual KxHTMLNode GetNextSibling() const override;
		virtual KxHTMLNode GetFirstChild() const override;
		virtual KxHTMLNode GetLastChild() const override;
};

class KX_API KxHTMLDocument: public KxHTMLNode
{
	private:
		wxMemoryBuffer m_Buffer;
		void* m_ParserOutput = NULL;
		void* m_ParserOptions = NULL;

	private:
		void Init();
		void Load();
		void UnLoad();

		virtual const void* GetNode() const override;
		virtual void SetNode(const void* node) override
		{
		}

	public:
		KxHTMLDocument(const wxString& htmlText = wxEmptyString);
		KxHTMLDocument(wxInputStream& stream);
		virtual ~KxHTMLDocument();

	public:
		virtual bool IsOK() const override;

		bool Load(const wxString& htmlText);
		bool Load(wxInputStream& stream);
		bool Save(wxOutputStream& stream) const;
		wxString Save() const;
};

enum KxHTML_NodeType
{
	KxHTML_NODE_INVALID = -1,

	KxHTML_NODE_DOCUMENT = 0, // GumboNodeType::GUMBO_NODE_DOCUMENT,
	KxHTML_NODE_ELEMENT = 1, // GumboNodeType::GUMBO_NODE_ELEMENT,
	KxHTML_NODE_TEXT = 2, // GumboNodeType::GUMBO_NODE_TEXT,
	KxHTML_NODE_CDATA = 3, // GumboNodeType::GUMBO_NODE_CDATA,
	KxHTML_NODE_COMMENT = 4, // GumboNodeType::GUMBO_NODE_COMMENT,
	KxHTML_NODE_TEMPLATE = 5, // GumboNodeType::GUMBO_NODE_TEMPLATE,
	KxHTML_NODE_WHITESPACE = 6, // GumboNodeType::GUMBO_NODE_WHITESPACE,
};
enum KxHTML_TagType
{
	KxHTML_TAG_HTML,
	KxHTML_TAG_HEAD,
	KxHTML_TAG_TITLE,
	KxHTML_TAG_BASE,
	KxHTML_TAG_LINK,
	KxHTML_TAG_META,
	KxHTML_TAG_STYLE,
	KxHTML_TAG_SCRIPT,
	KxHTML_TAG_NOSCRIPT,
	KxHTML_TAG_TEMPLATE,
	KxHTML_TAG_BODY,
	KxHTML_TAG_ARTICLE,
	KxHTML_TAG_SECTION,
	KxHTML_TAG_NAV,
	KxHTML_TAG_ASIDE,
	KxHTML_TAG_H1,
	KxHTML_TAG_H2,
	KxHTML_TAG_H3,
	KxHTML_TAG_H4,
	KxHTML_TAG_H5,
	KxHTML_TAG_H6,
	KxHTML_TAG_HGROUP,
	KxHTML_TAG_HEADER,
	KxHTML_TAG_FOOTER,
	KxHTML_TAG_ADDRESS,
	KxHTML_TAG_P,
	KxHTML_TAG_HR,
	KxHTML_TAG_PRE,
	KxHTML_TAG_BLOCKQUOTE,
	KxHTML_TAG_OL,
	KxHTML_TAG_UL,
	KxHTML_TAG_LI,
	KxHTML_TAG_DL,
	KxHTML_TAG_DT,
	KxHTML_TAG_DD,
	KxHTML_TAG_FIGURE,
	KxHTML_TAG_FIGCAPTION,
	KxHTML_TAG_MAIN,
	KxHTML_TAG_DIV,
	KxHTML_TAG_A,
	KxHTML_TAG_EM,
	KxHTML_TAG_STRONG,
	KxHTML_TAG_SMALL,
	KxHTML_TAG_S,
	KxHTML_TAG_CITE,
	KxHTML_TAG_Q,
	KxHTML_TAG_DFN,
	KxHTML_TAG_ABBR,
	KxHTML_TAG_DATA,
	KxHTML_TAG_TIME,
	KxHTML_TAG_CODE,
	KxHTML_TAG_VAR,
	KxHTML_TAG_SAMP,
	KxHTML_TAG_KBD,
	KxHTML_TAG_SUB,
	KxHTML_TAG_SUP,
	KxHTML_TAG_I,
	KxHTML_TAG_B,
	KxHTML_TAG_U,
	KxHTML_TAG_MARK,
	KxHTML_TAG_RUBY,
	KxHTML_TAG_RT,
	KxHTML_TAG_RP,
	KxHTML_TAG_BDI,
	KxHTML_TAG_BDO,
	KxHTML_TAG_SPAN,
	KxHTML_TAG_BR,
	KxHTML_TAG_WBR,
	KxHTML_TAG_INS,
	KxHTML_TAG_DEL,
	KxHTML_TAG_IMAGE,
	KxHTML_TAG_IMG,
	KxHTML_TAG_IFRAME,
	KxHTML_TAG_EMBED,
	KxHTML_TAG_OBJECT,
	KxHTML_TAG_PARAM,
	KxHTML_TAG_VIDEO,
	KxHTML_TAG_AUDIO,
	KxHTML_TAG_SOURCE,
	KxHTML_TAG_TRACK,
	KxHTML_TAG_CANVAS,
	KxHTML_TAG_MAP,
	KxHTML_TAG_AREA,
	KxHTML_TAG_MATH,
	KxHTML_TAG_MI,
	KxHTML_TAG_MO,
	KxHTML_TAG_MN,
	KxHTML_TAG_MS,
	KxHTML_TAG_MTEXT,
	KxHTML_TAG_MGLYPH,
	KxHTML_TAG_MALIGNMARK,
	KxHTML_TAG_ANNOTATION_XML,
	KxHTML_TAG_SVG,
	KxHTML_TAG_FOREIGNOBJECT,
	KxHTML_TAG_DESC,
	KxHTML_TAG_TABLE,
	KxHTML_TAG_CAPTION,
	KxHTML_TAG_COLGROUP,
	KxHTML_TAG_COL,
	KxHTML_TAG_TBODY,
	KxHTML_TAG_THEAD,
	KxHTML_TAG_TFOOT,
	KxHTML_TAG_TR,
	KxHTML_TAG_TD,
	KxHTML_TAG_TH,
	KxHTML_TAG_FORM,
	KxHTML_TAG_FIELDSET,
	KxHTML_TAG_LEGEND,
	KxHTML_TAG_LABEL,
	KxHTML_TAG_INPUT,
	KxHTML_TAG_BUTTON,
	KxHTML_TAG_SELECT,
	KxHTML_TAG_DATALIST,
	KxHTML_TAG_OPTGROUP,
	KxHTML_TAG_OPTION,
	KxHTML_TAG_TEXTAREA,
	KxHTML_TAG_KEYGEN,
	KxHTML_TAG_OUTPUT,
	KxHTML_TAG_PROGRESS,
	KxHTML_TAG_METER,
	KxHTML_TAG_DETAILS,
	KxHTML_TAG_SUMMARY,
	KxHTML_TAG_MENU,
	KxHTML_TAG_MENUITEM,
	KxHTML_TAG_APPLET,
	KxHTML_TAG_ACRONYM,
	KxHTML_TAG_BGSOUND,
	KxHTML_TAG_DIR,
	KxHTML_TAG_FRAME,
	KxHTML_TAG_FRAMESET,
	KxHTML_TAG_NOFRAMES,
	KxHTML_TAG_ISINDEX,
	KxHTML_TAG_LISTING,
	KxHTML_TAG_XMP,
	KxHTML_TAG_NEXTID,
	KxHTML_TAG_NOEMBED,
	KxHTML_TAG_PLAINTEXT,
	KxHTML_TAG_RB,
	KxHTML_TAG_STRIKE,
	KxHTML_TAG_BASEFONT,
	KxHTML_TAG_BIG,
	KxHTML_TAG_BLINK,
	KxHTML_TAG_CENTER,
	KxHTML_TAG_FONT,
	KxHTML_TAG_MARQUEE,
	KxHTML_TAG_MULTICOL,
	KxHTML_TAG_NOBR,
	KxHTML_TAG_SPACER,
	KxHTML_TAG_TT,
	KxHTML_TAG_RTC,

	KxHTML_TAG_UNKNOWN,
	KxHTML_TAG_LAST,
};
