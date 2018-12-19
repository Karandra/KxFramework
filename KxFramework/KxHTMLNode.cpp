#include "KxStdAfx.h"
#include "KxFramework/KxHTML.h"
#include "KxFramework/KxUtility.h"

#pragma warning(disable: 4005) // macro redefinition
#include "gumbo.h"

namespace
{
	const GumboNode* ToGumboNode(const void* node)
	{
		return reinterpret_cast<const GumboNode*>(node);
	}
}

namespace NodeInternals
{
	std::string gumbo_ex_cleantext(GumboNode* node);
	std::string gumbo_ex_serialize(GumboNode* node);

	wxString GetTagName(GumboTag tagType)
	{
		return gumbo_normalized_tagname(tagType);
	}
	wxString GetTagName(const GumboNode* node)
	{
		switch (node->type)
		{
			case GUMBO_NODE_DOCUMENT:
			{
				return GetTagName(GumboTag::GUMBO_TAG_HTML);
			}
			case GUMBO_NODE_ELEMENT:
			{
				GumboStringPiece stringPiece = node->v.element.original_tag;
				if (stringPiece.data && stringPiece.length != 0)
				{
					gumbo_tag_from_original_text(&stringPiece);
					
					wxString tagName = wxString::FromUTF8Unchecked(stringPiece.data, stringPiece.length);
					KxString::MakeLower(tagName);
					return tagName;
				}
				return GetTagName(node->v.element.tag);
			}
		};
		return GetTagName(GumboTag::GUMBO_TAG_UNKNOWN);
	}
	
	bool IsFullNode(const GumboNode* node)
	{
		return node->type == GumboNodeType::GUMBO_NODE_DOCUMENT || node->type == GumboNodeType::GUMBO_NODE_ELEMENT;
	}
	
	const GumboNode* GetNodeAt(const GumboVector* nodes, size_t i)
	{
		if (i < nodes->length)
		{
			return (GumboNode*)nodes->data[i];
		}
		return nullptr;
	}
	const GumboAttribute* GetAttribute(const GumboNode* node, const wxString& name)
	{
		if (node->type == GUMBO_NODE_ELEMENT)
		{
			auto utf8 = name.ToUTF8();
			return (GumboAttribute*)gumbo_get_attribute(&node->v.element.attributes, utf8.data());
		}
		return nullptr;
	}
	const GumboAttribute** GetAttributes(const GumboNode* node)
	{
		if (node->type == GUMBO_NODE_ELEMENT)
		{
			return (const GumboAttribute**)node->v.element.attributes.data;
		}
		return nullptr;
	}
	size_t GetAttributesCount(const GumboNode* node)
	{
		if (node->type == GUMBO_NODE_ELEMENT)
		{
			return node->v.element.attributes.length;
		}
		return 0;
	}
	
	const GumboVector* GetChildren(const GumboNode* node)
	{
		const GumboVector* children = nullptr;
		if (node->type == GUMBO_NODE_DOCUMENT)
		{
			children = &node->v.document.children;
		}
		else if (node->type == GUMBO_NODE_ELEMENT)
		{
			children = &node->v.element.children;
		}
		return children;
	}
	size_t GetChildrenCount(const GumboNode* node)
	{
		const GumboVector* children = GetChildren(node);
		if (children)
		{
			return children->length;
		}
		return 0;
	}
	
	const GumboNode* GetElementByAttribute(const GumboNode* node, const wxString& name, const wxString& value)
	{
		const GumboAttribute* attributes = GetAttribute(node, name);
		if (attributes && attributes->value == value)
		{
			return node;
		}
		else
		{
			size_t count = GetChildrenCount(node);
			for (size_t i = 0; i < count; i++)
			{
				const GumboVector* children = GetChildren(node);
				if (children)
				{
					const GumboNode* foundNode = GetElementByAttribute((GumboNode*)children->data[i], name, value);
					if (foundNode)
					{
						return foundNode;
					}
				}
			}
		}

		return nullptr;
	}
	const GumboNode* GetElementByTag(const GumboNode* node, const wxString& desiredTagName)
	{
		wxString tagName = GetTagName(node);
		if (tagName.IsSameAs(desiredTagName, false))
		{
			return node;
		}
		else
		{
			size_t count = GetChildrenCount(node);
			for (size_t i = 0; i < count; i++)
			{
				const GumboVector* children = GetChildren(node);
				if (children)
				{
					const GumboNode* foundNode = GetElementByTag((GumboNode*)children->data[i], desiredTagName);
					if (foundNode)
					{
						return foundNode;
					}
				}
			}
		}

		return nullptr;
	}
	const GumboNode* GetParent(const GumboNode* node)
	{
		return node->parent;
	}
};

const KxHTMLNode KxHTMLNode::NullNode = KxHTMLNode();

wxString KxHTMLNode::DoGetValue(const wxString& defaultValue) const
{
	auto node = GetNode();
	if (node && NodeInternals::IsFullNode(ToGumboNode(node)))
	{
		const GumboVector* children = NodeInternals::GetChildren(ToGumboNode(node));
		if (children && children->length == 1)
		{
			return KxHTMLNode(NodeInternals::GetNodeAt(children, 0), GetDocumentNode()).GetValue();
		}
	}
	else if (node)
	{
		return ToGumboNode(node)->v.text.text;
	}
	return defaultValue;
}
bool KxHTMLNode::DoSetValue(const wxString& value, bool isCDATA)
{
	return false;
}

wxString KxHTMLNode::DoGetAttribute(const wxString& name, const wxString& defaultValue) const
{
	auto node = GetNode();
	if (node)
	{
		const GumboAttribute* attribute = NodeInternals::GetAttribute(ToGumboNode(node), name);
		if (attribute && attribute->value)
		{
			return attribute->value;
		}
	}
	return defaultValue;
}
bool KxHTMLNode::DoSetAttribute(const wxString& name, const wxString& value)
{
	return false;
}

KxHTMLNode KxHTMLNode::QueryElement(const wxString& XPath) const
{
	return NullNode;
}
KxHTMLNode KxHTMLNode::QueryOrCreateElement(const wxString& XPath)
{
	return NullNode;
}

size_t KxHTMLNode::GetIndexWithinParent() const
{
	return ToGumboNode(m_Node)->index_within_parent;
}
bool KxHTMLNode::IsFullNode() const
{
	return NodeInternals::IsFullNode(ToGumboNode(GetNode()));
}

wxString KxHTMLNode::GetHTML() const
{
	auto node = GetNode();
	std::string buffer;
	if (node && NodeInternals::IsFullNode(ToGumboNode(node)))
	{
		buffer = NodeInternals::gumbo_ex_serialize(const_cast<GumboNode*>(ToGumboNode(node)));
	}
	else if (node)
	{
		auto utf8 = GetValue().ToUTF8();
		buffer.assign(utf8.data(), utf8.length());
	}
	return ToWxString(buffer);
}
wxString KxHTMLNode::GetValueText() const
{
	auto node = GetNode();
	if (node)
	{
		if (ToGumboNode(node)->type == GUMBO_NODE_TEXT)
		{
			return wxString(ToGumboNode(node)->v.text.text);
		}
		else
		{
			return ToWxString(NodeInternals::gumbo_ex_cleantext(const_cast<GumboNode*>(ToGumboNode(node))));
		}
	}
	return wxEmptyString;
}
wxString KxHTMLNode::GetName() const
{
	auto node = GetNode();
	if (node && NodeInternals::IsFullNode(ToGumboNode(node)))
	{
		return NodeInternals::GetTagName(ToGumboNode(node));
	}
	return wxEmptyString;
}

KxHTML_NodeType KxHTMLNode::GetType() const
{
	auto node = GetNode();
	if (node)
	{
		return static_cast<KxHTML_NodeType>(ToGumboNode(node)->type);
	}
	return KxHTML_NODE_INVALID;
}
KxHTML_TagType KxHTMLNode::GetTagType() const
{
	if (auto node = GetNode())
	{
		switch (ToGumboNode(node)->type)
		{
			case GUMBO_NODE_DOCUMENT:
			{
				return KxHTML_TAG_HTML;
			}
			case GUMBO_NODE_ELEMENT:
			{
				return static_cast<KxHTML_TagType>(ToGumboNode(node)->v.element.tag);
			}
		};
	}
	return KxHTML_TAG_UNKNOWN;
}

bool KxHTMLNode::HasAttribute(const wxString& name) const
{
	auto node = GetNode();
	if (node)
	{
		return NodeInternals::GetAttribute(ToGumboNode(node), name) != nullptr;
	}
	return false;
}
KxStringVector KxHTMLNode::GetAttributes() const
{
	KxStringVector list;

	auto node = GetNode();
	if (node)
	{
		const GumboAttribute** attributes = NodeInternals::GetAttributes(ToGumboNode(node));
		if (attributes)
		{
			list.reserve(NodeInternals::GetAttributesCount(ToGumboNode(node)));
			for (size_t i = 0; i < NodeInternals::GetAttributesCount(ToGumboNode(node)); i++)
			{
				list.push_back(attributes[i]->name);
			}
		}
	}
	return list;
}
size_t KxHTMLNode::GetAttributeCount() const
{
	return NodeInternals::GetAttributesCount(ToGumboNode(GetNode()));
}

KxHTMLNode::NodeVector KxHTMLNode::GetChildren() const
{
	auto node = GetNode();

	NodeVector list;
	if (node)
	{
		auto children = NodeInternals::GetChildren(ToGumboNode(node));
		if (children)
		{
			list.reserve(children->length);
			for (size_t i = 0; i < NodeInternals::GetChildrenCount(ToGumboNode(node)); i++)
			{
				list.push_back(KxHTMLNode(NodeInternals::GetNodeAt(children, i), m_Document));
			}
		}
	}
	return list;
}
size_t KxHTMLNode::GetChildrenCount() const
{
	return NodeInternals::GetChildrenCount(ToGumboNode(GetNode()));
}

KxHTMLNode KxHTMLNode::GetElementByAttribute(const wxString& name, const wxString& value) const
{
	return KxHTMLNode(NodeInternals::GetElementByAttribute(ToGumboNode(GetNode()), name, value), m_Document);
}
KxHTMLNode KxHTMLNode::GetElementByTag(KxHTML_TagType tagType) const
{
	return KxHTMLNode(NodeInternals::GetElementByTag(ToGumboNode(GetNode()), NodeInternals::GetTagName(static_cast<GumboTag>(tagType))), m_Document);
}
KxHTMLNode KxHTMLNode::GetElementByTag(const wxString& tagName) const
{
	return KxHTMLNode(NodeInternals::GetElementByTag(ToGumboNode(GetNode()), KxString::ToLower(tagName)), m_Document);
}

KxHTMLNode KxHTMLNode::GetParent() const
{
	auto node = GetNode();
	if (node)
	{
		return KxHTMLNode(NodeInternals::GetParent(ToGumboNode(node)), m_Document);
	}
	return NullNode;
}
KxHTMLNode KxHTMLNode::GetPreviousSibling() const
{
	auto node = GetNode();
	if (node && ToGumboNode(node)->parent && ToGumboNode(node)->index_within_parent != -1 && ToGumboNode(node)->index_within_parent > 0)
	{
		const GumboVector* children = NodeInternals::GetChildren(ToGumboNode(node)->parent);
		if (children)
		{
			KxHTMLNode node(NodeInternals::GetNodeAt(children, ToGumboNode(node)->index_within_parent - 1), m_Document);
			if (node.IsOK())
			{
				return node;
			}
		}
	}
	return NullNode;
}
KxHTMLNode KxHTMLNode::GetNextSibling() const
{
	auto node = GetNode();
	if (node && ToGumboNode(node)->parent)
	{
		size_t max = NodeInternals::GetChildrenCount(ToGumboNode(node)->parent);
		if (ToGumboNode(node)->index_within_parent != -1 && ToGumboNode(node)->index_within_parent < max)
		{
			const GumboVector* children = NodeInternals::GetChildren(ToGumboNode(node)->parent);
			if (children)
			{
				const GumboNode* pNewNode = NodeInternals::GetNodeAt(children, ToGumboNode(node)->index_within_parent + 1);
				if (pNewNode)
				{
					return KxHTMLNode(pNewNode, m_Document);
				}
			}
		}
	}
	return NullNode;
}
KxHTMLNode KxHTMLNode::GetFirstChild() const
{
	auto node = GetNode();
	if (node)
	{
		auto children = NodeInternals::GetChildren(ToGumboNode(node));
		if (children && children->length != 0)
		{
			return KxHTMLNode(NodeInternals::GetNodeAt(children, 0), m_Document);
		}
	}
	return NullNode;
}
KxHTMLNode KxHTMLNode::GetLastChild() const
{
	auto node = GetNode();
	if (node)
	{
		auto children = NodeInternals::GetChildren(ToGumboNode(node));
		if (children && children->length != 0)
		{
			return KxHTMLNode(NodeInternals::GetNodeAt(children, children->length - 1), m_Document);
		}
	}
	return NullNode;
}
