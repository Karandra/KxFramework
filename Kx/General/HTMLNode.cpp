#include "stdafx.h"
#include "HTMLDocument.h"

#pragma warning(disable: 4005) // macro redefinition
#include "gumbo.h"

namespace
{
	const GumboNode* ToGumboNode(const void* node) noexcept
	{
		return reinterpret_cast<const GumboNode*>(node);
	}
	const GumboAttribute* ToGumboAttribute(const void* node) noexcept
	{
		return reinterpret_cast<const GumboAttribute*>(node);
	}
}

namespace KxFramework::HTML::Private
{
	std::string gumbo_ex_cleantext(GumboNode* node);
	std::string gumbo_ex_serialize(GumboNode* node);

	String GetTagName(GumboTag tagType)
	{
		return gumbo_normalized_tagname(tagType);
	}
	String GetTagName(const GumboNode* node)
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
					return String::FromUTF8(stringPiece.data, stringPiece.length).MakeLower();
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
			return ToGumboNode(nodes->data[i]);
		}
		return nullptr;
	}
	const GumboAttribute* GetAttribute(const GumboNode* node, const String& name)
	{
		if (node->type == GUMBO_NODE_ELEMENT)
		{
			auto utf8 = name.ToUTF8();
			return gumbo_get_attribute(&node->v.element.attributes, utf8.data());
		}
		return nullptr;
	}
	const GumboAttribute** GetAttributes(const GumboNode* node)
	{
		if (node->type == GUMBO_NODE_ELEMENT)
		{
			return const_cast<const GumboAttribute**>(reinterpret_cast<GumboAttribute**>(node->v.element.attributes.data));
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
	
	const GumboNode* GetElementByAttribute(const GumboNode* node, const String& name, const String& value)
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
	const GumboNode* GetElementByTag(const GumboNode* node, const String& desiredTagName)
	{
		String tagName = GetTagName(node);
		if (tagName.IsSameAs(desiredTagName, StringOpFlag::IgnoreCase))
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

namespace KxFramework
{
	std::optional<String> KxHTMLNode::DoGetValue() const
	{
		auto node = GetNode();
		if (node && HTML::Private::IsFullNode(ToGumboNode(node)))
		{
			const GumboVector* children = HTML::Private::GetChildren(ToGumboNode(node));
			if (children && children->length == 1)
			{
				return KxHTMLNode(HTML::Private::GetNodeAt(children, 0), &GetDocumentNode()).GetValue();
			}
		}
		else if (node)
		{
			return ToGumboNode(node)->v.text.text;
		}
		return {};
	}
	bool KxHTMLNode::DoSetValue(const String& value, WriteEmpty writeEmpty, AsCDATA asCDATA)
	{
		return false;
	}

	std::optional<String> KxHTMLNode::DoGetAttribute(const String& name) const
	{
		if (auto node = GetNode())
		{
			const GumboAttribute* attribute = HTML::Private::GetAttribute(ToGumboNode(node), name);
			if (attribute && attribute->value)
			{
				return attribute->value;
			}
		}
		return {};
	}
	bool KxHTMLNode::DoSetAttribute(const String& name, const String& value, WriteEmpty writeEmpty)
	{
		return false;
	}

	KxHTMLNode KxHTMLNode::QueryElement(const String& XPath) const
	{
		return {};
	}
	KxHTMLNode KxHTMLNode::ConstructElement(const String& XPath)
	{
		return {};
	}

	size_t KxHTMLNode::GetIndexWithinParent() const
	{
		return ToGumboNode(m_Node)->index_within_parent;
	}
	bool KxHTMLNode::IsFullNode() const
	{
		return HTML::Private::IsFullNode(ToGumboNode(GetNode()));
	}

	String KxHTMLNode::GetHTML() const
	{
		auto node = GetNode();
		if (node && HTML::Private::IsFullNode(ToGumboNode(node)))
		{
			String::FromUTF8(HTML::Private::gumbo_ex_serialize(const_cast<GumboNode*>(ToGumboNode(node))));
		}
		else if (node)
		{
			return GetValue();
		}
		return {};
	}
	String KxHTMLNode::GetValueText() const
	{
		auto node = GetNode();
		if (node)
		{
			if (ToGumboNode(node)->type == GUMBO_NODE_TEXT)
			{
				return String::FromUTF8(ToGumboNode(node)->v.text.text);
			}
			else
			{
				return String::FromUTF8(HTML::Private::gumbo_ex_cleantext(const_cast<GumboNode*>(ToGumboNode(node))));
			}
		}
		return {};
	}
	String KxHTMLNode::GetName() const
	{
		auto node = GetNode();
		if (node && HTML::Private::IsFullNode(ToGumboNode(node)))
		{
			return HTML::Private::GetTagName(ToGumboNode(node));
		}
		return {};
	}

	HTML::NodeType KxHTMLNode::GetType() const
	{
		auto node = GetNode();
		if (node)
		{
			return static_cast<NodeType>(ToGumboNode(node)->type);
		}
		return NodeType::None;
	}
	HTML::TagType KxHTMLNode::GetTagType() const
	{
		if (auto node = GetNode())
		{
			switch (ToGumboNode(node)->type)
			{
				case GUMBO_NODE_DOCUMENT:
				{
					return TagType::HTML;
				}
				case GUMBO_NODE_ELEMENT:
				{
					return static_cast<TagType>(ToGumboNode(node)->v.element.tag);
				}
			};
		}
		return TagType::UNKNOWN;
	}

	size_t KxHTMLNode::GetAttributeCount() const
	{
		return HTML::Private::GetAttributesCount(ToGumboNode(GetNode()));
	}
	size_t KxHTMLNode::EnumAttributeNames(std::function<bool(String)> func) const
	{
		if (auto node = GetNode())
		{
			if (const GumboAttribute** attributes = HTML::Private::GetAttributes(ToGumboNode(node)))
			{
				const size_t attributeCount = GetAttributeCount();

				size_t count = 0;
				for (size_t i = 0; i < attributeCount; i++)
				{
					count++;
					if (!std::invoke(func, String::FromUTF8(attributes[i]->name)))
					{
						break;
					}
				}
				return 0;
			}
		}
		return 0;
	}
	bool KxHTMLNode::HasAttribute(const String& name) const
	{
		if (auto node = GetNode())
		{
			return HTML::Private::GetAttribute(ToGumboNode(node), name);
		}
		return false;
	}

	size_t KxHTMLNode::GetChildrenCount() const
	{
		return HTML::Private::GetChildrenCount(ToGumboNode(GetNode()));
	}
	size_t KxHTMLNode::EnumChildren(std::function<bool(KxHTMLNode)> func) const
	{
		if (auto node = GetNode())
		{
			if (auto children = HTML::Private::GetChildren(ToGumboNode(node)))
			{
				size_t count = 0;
				for (size_t i = 0; i < children->length; i++)
				{
					count++;
					if (!std::invoke(func, KxHTMLNode(HTML::Private::GetNodeAt(children, i), m_Document)))
					{
						break;
					}
				}
				return count;
			}
		}
		return 0;
	}

	KxHTMLNode KxHTMLNode::GetElementByAttribute(const String& name, const String& value) const
	{
		return KxHTMLNode(HTML::Private::GetElementByAttribute(ToGumboNode(GetNode()), name, value), m_Document);
	}
	KxHTMLNode KxHTMLNode::GetElementByTag(TagType tagType) const
	{
		return KxHTMLNode(HTML::Private::GetElementByTag(ToGumboNode(GetNode()), HTML::Private::GetTagName(static_cast<GumboTag>(tagType))), m_Document);
	}
	KxHTMLNode KxHTMLNode::GetElementByTag(const String& tagName) const
	{
		return KxHTMLNode(HTML::Private::GetElementByTag(ToGumboNode(GetNode()), tagName.ToLower()), m_Document);
	}

	KxHTMLNode KxHTMLNode::GetParent() const
	{
		if (auto node = GetNode())
		{
			return KxHTMLNode(HTML::Private::GetParent(ToGumboNode(node)), m_Document);
		}
		return {};
	}
	KxHTMLNode KxHTMLNode::GetPreviousSibling() const
	{
		auto node = GetNode();
		if (node && ToGumboNode(node)->parent && ToGumboNode(node)->index_within_parent != -1 && ToGumboNode(node)->index_within_parent > 0)
		{
			if (const GumboVector* children = HTML::Private::GetChildren(ToGumboNode(node)->parent))
			{
				return KxHTMLNode(HTML::Private::GetNodeAt(children, ToGumboNode(node)->index_within_parent - 1), m_Document);
			}
		}
		return {};
	}
	KxHTMLNode KxHTMLNode::GetNextSibling() const
	{
		auto node = GetNode();
		if (node && ToGumboNode(node)->parent)
		{
			const size_t max = HTML::Private::GetChildrenCount(ToGumboNode(node)->parent);
			if (ToGumboNode(node)->index_within_parent != -1 && ToGumboNode(node)->index_within_parent < max)
			{
				if (const GumboVector* children = HTML::Private::GetChildren(ToGumboNode(node)->parent))
				{
					const GumboNode* sibling = HTML::Private::GetNodeAt(children, ToGumboNode(node)->index_within_parent + 1);
					return KxHTMLNode(sibling, m_Document);
				}
			}
		}
		return {};
	}
	KxHTMLNode KxHTMLNode::GetFirstChild() const
	{
		if (auto node = GetNode())
		{
			auto children = HTML::Private::GetChildren(ToGumboNode(node));
			if (children && children->length != 0)
			{
				return KxHTMLNode(HTML::Private::GetNodeAt(children, 0), m_Document);
			}
		}
		return {};
	}
	KxHTMLNode KxHTMLNode::GetLastChild() const
	{
		if (auto node = GetNode())
		{
			auto children = HTML::Private::GetChildren(ToGumboNode(node));
			if (children && children->length != 0)
			{
				return KxHTMLNode(HTML::Private::GetNodeAt(children, children->length - 1), m_Document);
			}
		}
		return {};
	}
}
