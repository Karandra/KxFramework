#include "KxStdAfx.h"
#include "Node.h"
#include "Element.h"
#include "SciterAPI/sciter-x-api.h"

namespace
{
	HNODE ToNode(void* handle)
	{
		return reinterpret_cast<HNODE>(handle);
	}
	
	template<class TNode = HELEMENT, class TFunc>
	KxSciter::Node DoGetNode(void* handle, TFunc&& func)
	{
		TNode node = nullptr;
		if (func(ToNode(handle), &node) == SCDOM_OK)
		{
			return KxSciter::Node(node);
		}
		return {};
	}

	template<class TNode = HELEMENT, class TFunc>
	KxSciter::Node DoCreateTextNode(TFunc&& func, wxStringView value)
	{
		HNODE nativeNode = nullptr;
		if (func(value.data(), value.length(), &nativeNode) == SCDOM_OK)
		{
			KxSciter::Node node;
			node.Attach(nativeNode);
			return node;
		}
		return {};
	}

	NODE_TYPE DoGetNodeType(void* handle)
	{
		UINT nodeType = std::numeric_limits<UINT>::max();
		::SciterNodeType(ToNode(handle), &nodeType);

		return static_cast<NODE_TYPE>(nodeType);
	}
	bool DoInsertNode(KxSciter::Node& thisNode, const KxSciter::Node& node, NODE_INS_TARGET mode)
	{
		return ::SciterNodeInsert(ToNode(thisNode.GetHandle()), mode, ToNode(node.GetHandle())) == SCDOM_OK;
	}
}

namespace KxSciter
{
	Node Node::CreateTextNode(wxStringView value)
	{
		return DoCreateTextNode(::SciterCreateTextNode, value);
	}
	Node Node::CreateCommentNode(wxStringView value)
	{
		return DoCreateTextNode(::SciterCreateCommentNode, value);
	}

	void Node::Acquire(void* handle)
	{
		if (::SciterNodeAddRef(ToNode(handle)) == SCDOM_OK)
		{
			m_Handle = handle;
		}
		else
		{
			m_Handle = nullptr;
		}
	}
	void Node::Release()
	{
		if (m_Handle)
		{
			::SciterNodeRelease(ToNode(m_Handle));
			m_Handle = nullptr;
		}
	}

	void Node::CopyFrom(const Node& other)
	{
		Release();
		Acquire(other.m_Handle);
	}
	void Node::MoveFrom(Node& other)
	{
		Release();
		m_Handle = other.m_Handle;
		other.m_Handle = nullptr;
	}

	bool Node::Attach(void* handle)
	{
		if (!IsOk())
		{
			m_Handle = nullptr;
			return true;
		}
		return false;
	}
	void* Node::Detach()
	{
		void* handle = m_Handle;
		m_Handle = nullptr;

		if (::SciterNodeRemove(ToNode(handle), FALSE) == SCDOM_OK)
		{
			return handle;
		}
		return nullptr;
	}
	bool Node::Remove()
	{
		if (::SciterNodeRemove(ToNode(m_Handle), TRUE) == SCDOM_OK)
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
		HELEMENT node = nullptr;
		::SciterNodeCastToElement(ToNode(m_Handle), &node);
		return Element(node);
	}
	
	Node Node::GetParent() const
	{
		return DoGetNode(m_Handle, ::SciterNodeParent);
	}
	Node Node::GetPrevSibling() const
	{
		return DoGetNode<HNODE>(m_Handle, ::SciterNodePrevSibling);
	}
	Node Node::GetNextSibling() const
	{
		return DoGetNode<HNODE>(m_Handle, ::SciterNodeNextSibling);
	}
	Node Node::GetFirstChild() const
	{
		return DoGetNode<HNODE>(m_Handle, ::SciterNodeFirstChild);
	}
	Node Node::GetLastChild() const
	{
		return DoGetNode<HNODE>(m_Handle, ::SciterNodeLastChild);
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
		::SciterNodeChildrenCount(ToNode(m_Handle), &count);

		return count;
	}
	Node Node::GetChildAt(size_t index) const
	{
		HNODE node = nullptr;
		if (::SciterNodeNthChild(ToNode(m_Handle), index, &node) == SCDOM_OK)
		{
			return Node(node);
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
		::SciterNodeGetText(ToNode(m_Handle), [](LPCWSTR value, UINT length, LPVOID context)
		{
			reinterpret_cast<wxString*>(context)->assign(value, length);
		}, &result);
		return result;
	}
	bool Node::SetValue(wxStringView value) const
	{
		return ::SciterNodeSetText(ToNode(m_Handle), value.data(), value.length()) == SCDOM_OK;
	}
}
