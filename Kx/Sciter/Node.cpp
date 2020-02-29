#include "KxStdAfx.h"
#include "Node.h"
#include "Element.h"
#include "Internal.h"
#include "SciterAPI.h"

namespace KxSciter
{
	template<class TFunc>
	Node DoGetNode(NodeHandle* handle, TFunc&& func)
	{
		HNODE node = nullptr;
		if (func(ToSciterNode(handle), &node) == SCDOM_OK)
		{
			return FromSciterNode(node);
		}
		return {};
	}

	template<class TNode = HELEMENT, class TFunc>
	Node DoCreateTextNode(TFunc&& func, const wxString& value)
	{
		HNODE nativeNode = nullptr;
		if (func(value.wc_str(), value.length(), &nativeNode) == SCDOM_OK)
		{
			Node node;
			node.AttachHandle(FromSciterNode(nativeNode));
			return node;
		}
		return {};
	}

	NODE_TYPE DoGetNodeType(NodeHandle* handle)
	{
		UINT nodeType = std::numeric_limits<UINT>::max();
		GetSciterAPI()->SciterNodeType(ToSciterNode(handle), &nodeType);

		return static_cast<NODE_TYPE>(nodeType);
	}
	bool DoInsertNode(KxSciter::Node& thisNode, const Node& node, NODE_INS_TARGET mode)
	{
		return GetSciterAPI()->SciterNodeInsert(ToSciterNode(thisNode.GetHandle()), mode, ToSciterNode(node.GetHandle())) == SCDOM_OK;
	}
}

namespace KxSciter
{
	Node Node::CreateTextNode(const wxString& value)
	{
		return DoCreateTextNode(GetSciterAPI()->SciterCreateTextNode, value);
	}
	Node Node::CreateCommentNode(const wxString& value)
	{
		return DoCreateTextNode(GetSciterAPI()->SciterCreateCommentNode, value);
	}

	bool Node::DoAcquire(NodeHandle* handle)
	{
		return GetSciterAPI()->SciterNodeAddRef(ToSciterNode(handle)) == SCDOM_OK;
	}
	void Node::DoRelease()
	{
		GetSciterAPI()->SciterNodeRelease(ToSciterNode(m_Handle));
	}

	bool Node::Detach()
	{
		return GetSciterAPI()->SciterNodeRemove(ToSciterNode(m_Handle), FALSE) == SCDOM_OK;
	}
	bool Node::Remove()
	{
		if (GetSciterAPI()->SciterNodeRemove(ToSciterNode(m_Handle), TRUE) == SCDOM_OK)
		{
			Release();
			return true;
		}
		return false;
	}

	bool Node::IsText() const
	{
		return DoGetNodeType(m_Handle) == NODE_TYPE::NT_TEXT;
	}
	bool Node::IsElement() const
	{
		return DoGetNodeType(m_Handle) == NODE_TYPE::NT_ELEMENT;
	}
	bool Node::IsComment() const
	{
		return DoGetNodeType(m_Handle) == NODE_TYPE::NT_COMMENT;
	}

	Element Node::ToElement() const
	{
		HELEMENT element = nullptr;
		GetSciterAPI()->SciterNodeCastToElement(ToSciterNode(m_Handle), &element);
		return reinterpret_cast<ElementHandle*>(element);
	}
	
	Node Node::GetParent() const
	{
		HELEMENT element = nullptr;
		if (GetSciterAPI()->SciterNodeParent(ToSciterNode(m_Handle), &element) == SCDOM_OK)
		{
			return Element(reinterpret_cast<ElementHandle*>(element)).ToNode();
		}
		return {};
	}
	Node Node::GetPrevSibling() const
	{
		return DoGetNode(m_Handle, GetSciterAPI()->SciterNodePrevSibling);
	}
	Node Node::GetNextSibling() const
	{
		return DoGetNode(m_Handle, GetSciterAPI()->SciterNodeNextSibling);
	}
	Node Node::GetFirstChild() const
	{
		return DoGetNode(m_Handle, GetSciterAPI()->SciterNodeFirstChild);
	}
	Node Node::GetLastChild() const
	{
		return DoGetNode(m_Handle, GetSciterAPI()->SciterNodeLastChild);
	}

	size_t Node::GetIndexWithinParent() const
	{
		const size_t count = GetChildrenCount();
		for (size_t i = 0; i < count; i++)
		{
			if (GetChildAt(i) == *this)
			{
				return i;
			}
		}
		return 0;
	}
	size_t Node::GetChildrenCount() const
	{
		UINT count = 0;
		GetSciterAPI()->SciterNodeChildrenCount(ToSciterNode(m_Handle), &count);

		return count;
	}
	Node Node::GetChildAt(size_t index) const
	{
		HNODE node = nullptr;
		if (GetSciterAPI()->SciterNodeNthChild(ToSciterNode(m_Handle), index, &node) == SCDOM_OK)
		{
			return FromSciterNode(node);
		}
		return {};
	}

	bool Node::Append(const Node& node)
	{
		return DoInsertNode(*this, node, NODE_INS_TARGET::NIT_APPEND);
	}
	bool Node::Prepend(const Node& node)
	{
		return DoInsertNode(*this, node, NODE_INS_TARGET::NIT_PREPEND);
	}
	bool Node::InsertBefore(const Node& node)
	{
		return DoInsertNode(*this, node, NODE_INS_TARGET::NIT_BEFORE);
	}
	bool Node::InsertAfter(const Node& node)
	{
		return DoInsertNode(*this, node, NODE_INS_TARGET::NIT_AFTER);
	}

	wxString Node::GetValue() const
	{
		wxString result;
		GetSciterAPI()->SciterNodeGetText(ToSciterNode(m_Handle), [](LPCWSTR value, UINT length, LPVOID context)
		{
			reinterpret_cast<wxString*>(context)->assign(value, length);
		}, &result);
		return result;
	}
	bool Node::SetValue(wxStringView value) const
	{
		return GetSciterAPI()->SciterNodeSetText(ToSciterNode(m_Handle), value.data(), value.length()) == SCDOM_OK;
	}
}
