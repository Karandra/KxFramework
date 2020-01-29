#include "KxStdAfx.h"
#include "Element.h"
#include "Node.h"
#include "SciterAPI/sciter-x-api.h"

namespace
{
	HELEMENT ToElement(void* handle)
	{
		return reinterpret_cast<HELEMENT>(handle);
	}

	template<class TFunc>
	KxSciter::Element DoGetElemenet(void* handle, TFunc&& func)
	{
		HELEMENT node = nullptr;
		if (func(ToElement(handle), &node) == SCDOM_OK)
		{
			return KxSciter::Element(node);
		}
		return {};
	}

	void ExtractWxString(LPCWSTR value, UINT length, LPVOID context)
	{
		reinterpret_cast<wxString*>(context)->assign(value, length);
	}
}

namespace KxSciter
{
	void Element::Acquire(void* handle)
	{
		if (::Sciter_UseElement(ToElement(m_Handle)) == SCDOM_OK)
		{
			m_Handle = handle;
		}
		else
		{
			m_Handle = nullptr;
		}
	}
	void Element::Release()
	{
		if (m_Handle)
		{
			::Sciter_UnuseElement(ToElement(m_Handle));
			m_Handle = nullptr;
		}
	}

	void Element::CopyFrom(const Element& other)
	{
		Release();
		Acquire(other.m_Handle);
	}
	void Element::MoveFrom(Element& other)
	{
		Release();
		m_Handle = other.m_Handle;
		other.m_Handle = nullptr;
	}

	bool Element::Attach(void* handle)
	{
		if (!IsOk())
		{
			m_Handle = nullptr;
			return true;
		}
		return false;
	}
	void* Element::Detach()
	{
		void* handle = m_Handle;
		m_Handle = nullptr;

		if (::SciterDetachElement(ToElement(handle)))
		{
			return handle;
		}
		return nullptr;
	}
	bool Element::Remove()
	{
		if (::SciterDeleteElement(ToElement(m_Handle)) == SCDOM_OK)
		{
			Release();
			return true;
		}
		return false;
	}

	Node Element::ToNode() const
	{
		HNODE node = nullptr;
		::SciterNodeCastFromElement(ToElement(m_Handle), &node);
		return Node(node);
	}
	Element Element::Clone() const
	{
		HELEMENT nativeNode = nullptr;
		if (::SciterCloneElement(ToElement(m_Handle), &nativeNode) == SCDOM_OK)
		{
			Element node;
			node.Attach(nativeNode);
			return node;
		}
		return {};
	}

	Element Element::GetParent() const
	{
		return DoGetElemenet(m_Handle, ::SciterGetParentElement);
	}
	Element Element::GetPrevSibling() const
	{
		const size_t index = GetIndexWithinParent();
		const Element parent = GetParent();

		if (parent && index != 0)
		{
			return parent.GetChildAt(index - 1);
		}
		return {};
	}
	Element Element::GetNextSibling() const
	{
		const size_t index = GetIndexWithinParent();
		const Element parent = GetParent();

		if (parent && index + 1 < GetChildrenCount())
		{
			return parent.GetChildAt(index + 1);
		}
		return {};
	}
	Element Element::GetFirstChild() const
	{
		return GetChildAt(0);
	}
	Element Element::GetLastChild() const
	{
		size_t count = GetChildrenCount();
		if (count != 0)
		{
			return GetChildAt(count - 1);
		}
		return {};
	}

	size_t Element::GetIndexWithinParent() const
	{
		UINT index = std::numeric_limits<size_t>::max();
		::SciterGetElementIndex(ToElement(m_Handle), &index);
		return index;
	}
	size_t Element::GetChildrenCount() const
	{
		UINT count = 0;
		::SciterGetChildrenCount(ToElement(m_Handle), &count);

		return count;
	}
	Element Element::GetChildAt(size_t index) const
	{
		HELEMENT node = nullptr;
		if (::SciterGetNthChild(ToElement(m_Handle), index, &node) == SCDOM_OK)
		{
			return Element(node);
		}
		return {};
	}

	bool Element::Append(const Element& node)
	{
		return InsertAt(node, std::numeric_limits<UINT>::max());
	}
	bool Element::Prepend(const Element& node)
	{
		const size_t index = GetIndexWithinParent();
		return InsertAt(node, index != 0 ? index - 1 : 0);
	}
	bool Element::InsertAt(const Element& node, size_t index)
	{
		return ::SciterInsertElement(ToElement(node.m_Handle), ToElement(m_Handle), index) == SCDOM_OK;
	}
	bool Element::InsertBefore(const Element& node)
	{
		const size_t index = node.GetIndexWithinParent();
		return InsertAt(node, index != 0 ? index - 1 : 0);
	}
	bool Element::InsertAfter(const Element& node)
	{
		const size_t index = node.GetIndexWithinParent();
		return InsertAt(node, index + 1);
	}

	wxString Element::GetValue() const
	{
		wxString result;
		::SciterGetElementTextCB(ToElement(m_Handle), ExtractWxString, &result);
		return result;
	}
	bool Element::SetValue(wxStringView value) const
	{
		return ::SciterSetElementText(ToElement(m_Handle), value.data(), value.length()) == SCDOM_OK;
	}

	wxString Element::GetStyleAttribute(const wxString& name) const
	{
		wxString result;

		auto nameUTF8 = name.ToUTF8();
		::SciterGetStyleAttributeCB(ToElement(m_Handle), nameUTF8.data(), ExtractWxString, &result);
		return result;
	}
	bool Element::SetStyleAttribute(const wxString& name, const wxString& value)
	{
		auto nameUTF8 = name.ToUTF8();
		return ::SciterSetStyleAttribute(ToElement(m_Handle), nameUTF8.data(), value.wc_str()) == SCDOM_OK;
	}
}
