#include "KxStdAfx.h"
#include "KxFramework/KxHTML.h"
#include "KxFramework/KxUtility.h"

#pragma warning(disable: 4005)
#include "gumbo.h"

#define NODE(p)		reinterpret_cast<const GumboNode*>((p))

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
		return NULL;
	}
	const GumboAttribute* GetAttribute(const GumboNode* node, const wxString& name)
	{
		if (node->type == GUMBO_NODE_ELEMENT)
		{
			auto utf8 = name.ToUTF8();
			return (GumboAttribute*)gumbo_get_attribute(&node->v.element.attributes, utf8.data());
		}
		return NULL;
	}
	const GumboAttribute** GetAttributes(const GumboNode* node)
	{
		if (node->type == GUMBO_NODE_ELEMENT)
		{
			return (const GumboAttribute**)node->v.element.attributes.data;
		}
		return NULL;
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
		const GumboVector* children = NULL;
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

		return NULL;
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

		return NULL;
	}
	const GumboNode* GetParent(const GumboNode* node)
	{
		return node->parent;
	}
};

const KxHTMLNode KxHTMLNode::NullNode = KxHTMLNode();

bool KxHTMLNode::IsFullNode() const
{
	return NodeInternals::IsFullNode(NODE(GetNode()));
}
KxHTMLNode KxHTMLNode::QueryElement(const wxString& XPath) const
{
	return NullNode;
	#if 0
	static const wxString XPathDelimiter = "::";
	std::vector<const KxHTMLNode*> nodes;

	const KxHTMLNode* currentNode = NULL;
	try
	{
		currentNode = nodes.emplace_back(this);
		const KxHTMLNode* previousNode = currentNode;
		KxStringVector pathArray = KxString::Split(XPath, "/");
		if (!pathArray.empty())
		{
			for (size_t i = 0; i < pathArray.size() && currentNode && currentNode->IsOK(); i++)
			{
				// Save previous element
				previousNode = currentNode;

				// Extract index from name and remove it from path
				// point/x -> 1, point/x::2 -> 2, point/y::0 -> 1, point/z::-7 -> 1
				int requiredLevel = m_Document->ExtractIndexFromName(pathArray[i], XPathDelimiter);
				const wxString& elementName = pathArray[i];

				// Get level 1
				currentNode = nodes.emplace_back(new KxHTMLNode(currentNode->GetFirstChild()));
				while (currentNode && currentNode->GetName() != elementName)
				{
					currentNode = nodes.emplace_back(new KxHTMLNode(currentNode->GetNextSibling()));
				}

				// We need to go down by "index" elements
				if (requiredLevel != 1 && currentNode && currentNode->IsOK())
				{
					for (int level = 1; level < requiredLevel && currentNode && currentNode->IsOK(); level++)
					{
						// Get next level
						while (currentNode && currentNode->GetName() != elementName)
						{
							currentNode = nodes.emplace_back(new KxHTMLNode(currentNode->GetNextSibling()));
						}
					}
				}
			}
		}
	}
	catch (...)
	{
	}

	for (const KxHTMLNode* node: nodes)
	{
		if (node != this)
		{
			delete node;
		}
	}
	return KxHTMLNode(*currentNode);
	#endif
}

size_t KxHTMLNode::GetIndexWithinParent() const
{
	return NODE(m_Node)->index_within_parent;
}

wxString KxHTMLNode::GetHTML() const
{
	auto node = GetNode();
	std::string buffer;
	if (node && NodeInternals::IsFullNode(NODE(node)))
	{
		buffer = NodeInternals::gumbo_ex_serialize(const_cast<GumboNode*>(NODE(node)));
	}
	else if (node)
	{
		buffer = GetValue();
	}
	return ToWxString(buffer);
}
wxString KxHTMLNode::GetValueText() const
{
	auto node = GetNode();
	if (node)
	{
		if (NODE(node)->type == GUMBO_NODE_TEXT)
		{
			return wxString(NODE(node)->v.text.text);
		}
		else
		{
			return ToWxString(NodeInternals::gumbo_ex_cleantext(const_cast<GumboNode*>(NODE(node))));
		}
	}
	return wxEmptyString;
}
wxString KxHTMLNode::GetName() const
{
	auto node = GetNode();
	if (node && NodeInternals::IsFullNode(NODE(node)))
	{
		return NodeInternals::GetTagName(NODE(node));
	}
	return wxEmptyString;
}
wxString KxHTMLNode::GetValue(const wxString& defaultValue) const
{
	auto node = GetNode();
	if (node && NodeInternals::IsFullNode(NODE(node)))
	{
		const GumboVector* children = NodeInternals::GetChildren(NODE(node));
		if (children && children->length == 1)
		{
			return KxHTMLNode(NodeInternals::GetNodeAt(children, 0), GetDocumentNode()).GetValue();
		}
	}
	else if (node)
	{
		return NODE(node)->v.text.text;
	}
	return defaultValue;
}
KxHTML_NodeType KxHTMLNode::GetType() const
{
	auto node = GetNode();
	if (node)
	{
		return static_cast<KxHTML_NodeType>(NODE(node)->type);
	}
	return KxHTML_NODE_INVALID;
}
KxHTML_TagType KxHTMLNode::GetTagType() const
{
	if (auto node = GetNode())
	{
		switch (NODE(node)->type)
		{
			case GUMBO_NODE_DOCUMENT:
			{
				return KxHTML_TAG_HTML;
			}
			case GUMBO_NODE_ELEMENT:
			{
				return static_cast<KxHTML_TagType>(NODE(node)->v.element.tag);
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
		return NodeInternals::GetAttribute(NODE(node), name) != NULL;
	}
	return false;
}
wxString KxHTMLNode::GetAttribute(const wxString& name, const wxString& defaultValue) const
{
	auto node = GetNode();
	if (node)
	{
		const GumboAttribute* attribute = NodeInternals::GetAttribute(NODE(node), name);
		if (attribute && attribute->value)
		{
			return attribute->value;
		}
	}
	return defaultValue;
}
KxStringVector KxHTMLNode::GetAttributes() const
{
	KxStringVector list;

	auto node = GetNode();
	if (node)
	{
		const GumboAttribute** attributes = NodeInternals::GetAttributes(NODE(node));
		if (attributes)
		{
			list.reserve(NodeInternals::GetAttributesCount(NODE(node)));
			for (size_t i = 0; i < NodeInternals::GetAttributesCount(NODE(node)); i++)
			{
				list.push_back(attributes[i]->name);
			}
		}
	}
	return list;
}
size_t KxHTMLNode::GetAttributeCount() const
{
	return NodeInternals::GetAttributesCount(NODE(GetNode()));
}

KxHTMLNode::NodeVector KxHTMLNode::GetChildren() const
{
	auto node = GetNode();

	NodeVector list;
	if (node)
	{
		auto children = NodeInternals::GetChildren(NODE(node));
		if (children)
		{
			list.reserve(children->length);
			for (size_t i = 0; i < NodeInternals::GetChildrenCount(NODE(node)); i++)
			{
				list.push_back(KxHTMLNode(NodeInternals::GetNodeAt(children, i), m_Document));
			}
		}
	}
	return list;
}
size_t KxHTMLNode::GetChildrenCount() const
{
	return NodeInternals::GetChildrenCount(NODE(GetNode()));
}

KxHTMLNode KxHTMLNode::GetElementByAttribute(const wxString& name, const wxString& value) const
{
	return KxHTMLNode(NodeInternals::GetElementByAttribute(NODE(GetNode()), name, value), m_Document);
}
KxHTMLNode KxHTMLNode::GetElementByTag(KxHTML_TagType tagType) const
{
	return KxHTMLNode(NodeInternals::GetElementByTag(NODE(GetNode()), NodeInternals::GetTagName(static_cast<GumboTag>(tagType))), m_Document);
}
KxHTMLNode KxHTMLNode::GetElementByTag(const wxString& tagName) const
{
	return KxHTMLNode(NodeInternals::GetElementByTag(NODE(GetNode()), KxString::ToLower(tagName)), m_Document);
}

KxHTMLNode KxHTMLNode::GetParent() const
{
	auto node = GetNode();
	if (node)
	{
		return KxHTMLNode(NodeInternals::GetParent(NODE(node)), m_Document);
	}
	return NullNode;
}
KxHTMLNode KxHTMLNode::GetPreviousSibling() const
{
	auto node = GetNode();
	if (node && NODE(node)->parent && NODE(node)->index_within_parent != -1 && NODE(node)->index_within_parent > 0)
	{
		const GumboVector* children = NodeInternals::GetChildren(NODE(node)->parent);
		if (children)
		{
			KxHTMLNode node(NodeInternals::GetNodeAt(children, NODE(node)->index_within_parent - 1), m_Document);
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
	if (node && NODE(node)->parent)
	{
		size_t max = NodeInternals::GetChildrenCount(NODE(node)->parent);
		if (NODE(node)->index_within_parent != -1 && NODE(node)->index_within_parent < max)
		{
			const GumboVector* children = NodeInternals::GetChildren(NODE(node)->parent);
			if (children)
			{
				const GumboNode* pNewNode = NodeInternals::GetNodeAt(children, NODE(node)->index_within_parent + 1);
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
		auto children = NodeInternals::GetChildren(NODE(node));
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
		auto children = NodeInternals::GetChildren(NODE(node));
		if (children && children->length != 0)
		{
			return KxHTMLNode(NodeInternals::GetNodeAt(children, children->length - 1), m_Document);
		}
	}
	return NullNode;
}
