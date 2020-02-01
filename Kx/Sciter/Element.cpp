#include "KxStdAfx.h"
#include "Element.h"
#include "Node.h"
#include "Host.h"
#include "KxFramework/KxUtility.h"
#include "SciterAPI/sciter-x-api.h"

#pragma warning(disable: 4312) // 'reinterpret_cast': conversion from 'UINT' to 'void *' of greater size

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

	bool DoCheckStateFlag(void* handle, ELEMENT_STATE_BITS flag)
	{
		UINT state = 0;
		::SciterGetElementState(ToElement(handle), &state);

		return state & flag;
	}

	void __stdcall ExtractWxString(const wchar_t* value, UINT length, void* context)
	{
		reinterpret_cast<wxString*>(context)->assign(value, length);
	}
	void __stdcall ExtractWxString(const char* value, UINT length, void* context)
	{
		*reinterpret_cast<wxString*>(context) = wxString::FromUTF8Unchecked(value, length);
	}
	void __stdcall ExtractWxString(const BYTE* value, UINT length, void* context)
	{
		ExtractWxString(reinterpret_cast<const char*>(value), length, context);
	}
}

namespace KxSciter
{
	Element Element::Create(const wxString& tagName, const wxString& value)
	{
		Element node;

		auto tagNameUTF8 = tagName.ToUTF8();
		HELEMENT nativeNode = nullptr;
		if (::SciterCreateElement(tagNameUTF8.data(), value.wc_str(), &nativeNode) == SCDOM_OK)
		{
			node.Attach(nativeNode);
		}
		return node;
	}

	void Element::Acquire(void* handle)
	{
		if (::Sciter_UseElement(ToElement(handle)) == SCDOM_OK)
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

	void* Element::GetUID() const
	{
		UINT id = 0;
		::SciterGetElementUID(ToElement(m_Handle), &id);
		return reinterpret_cast<void*>(id);
	}
	Host* Element::GetHost() const
	{
		HWND windowHandle = nullptr;
		::SciterGetElementHwnd(ToElement(m_Handle), &windowHandle, TRUE);
		if (windowHandle)
		{
			return reinterpret_cast<Host*>(::GetWindowLongPtrW(windowHandle, GWLP_USERDATA));
		}
		return nullptr;
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

	bool Element::Update(bool force)
	{
		return ::SciterUpdateElement(ToElement(m_Handle), force) == SCDOM_OK;
	}
	bool Element::UpdateRect(const wxRect& rect)
	{
		return ::SciterRefreshElementArea(ToElement(m_Handle), KxUtility::CopyRectToRECT(rect)) == SCDOM_OK;
	}

	wxRect Element::GetRect() const
	{
		RECT nativeRect = {};
		if (::SciterGetElementLocation(ToElement(m_Handle), &nativeRect, ELEMENT_AREAS::VIEW_RELATIVE) == SCDOM_OK)
		{
			return KxUtility::CopyRECTToRect(nativeRect);
		}
		return {};
	}
	wxPoint Element::GetPosition() const
	{
		return GetRect().GetPosition();
	}
	wxSize Element::GetSize() const
	{
		return GetRect().GetSize();
	}

	wxSize Element::GetMinSize() const
	{
		INT minWidth = 0;
		INT maxWidth = 0;
		INT height = 0;
		::SciterGetElementIntrinsicWidths(ToElement(m_Handle), &minWidth, &maxWidth);
		::SciterGetElementIntrinsicHeight(ToElement(m_Handle), minWidth, &height);

		return wxSize(minWidth, height);
	}
	wxSize Element::GetMaxSize() const
	{
		INT minWidth = 0;
		INT maxWidth = 0;
		INT height = 0;
		::SciterGetElementIntrinsicWidths(ToElement(m_Handle), &minWidth, &maxWidth);
		::SciterGetElementIntrinsicHeight(ToElement(m_Handle), maxWidth, &height);

		return wxSize(maxWidth, height);
	}

	bool Element::SetCapture()
	{
		return ::SciterSetCapture(ToElement(m_Handle)) == SCDOM_OK;
	}
	bool Element::ReleaseCapture()
	{
		return ::SciterReleaseCapture(ToElement(m_Handle)) == SCDOM_OK;
	}

	bool Element::IsFocusable() const
	{
		return DoCheckStateFlag(m_Handle, ELEMENT_STATE_BITS::STATE_FOCUSABLE);
	}
	bool Element::HasFocus() const
	{
		return DoCheckStateFlag(m_Handle, ELEMENT_STATE_BITS::STATE_FOCUS);
	}
	void Element::SetFocus()
	{
		::SciterSetElementState(ToElement(m_Handle), ELEMENT_STATE_BITS::STATE_FOCUS, 0, FALSE);
	}

	bool Element::IsHighlighted() const
	{
		if (Host* host = GetHost())
		{
			return host->GetHighlightedElement() == *this;
		}
		return false;
	}
	void Element::SetHighlighted()
	{
		if (Host* host = GetHost())
		{
			host->SetHighlightedElement(*this);
		}
	}

	void Element::ScrollIntoView(bool toTop)
	{
		UINT nativeFlags = 0;
		if (toTop)
		{
			nativeFlags |= SCITER_SCROLL_FLAGS::SCROLL_TO_TOP;
		}
		if (Host* host = GetHost(); host && host->IsSmoothScrollingEnabled())
		{
			nativeFlags |= SCITER_SCROLL_FLAGS::SCROLL_SMOOTH;
		}

		::SciterScrollToView(ToElement(m_Handle), nativeFlags);
	}
	wxPoint Element::GetScrollPos() const
	{
		POINT pos = {};
		if (::SciterGetScrollInfo(ToElement(m_Handle), &pos, nullptr, nullptr) == SCDOM_OK)
		{
			return wxPoint(pos.x, pos.y);
		}
		return wxDefaultPosition;
	}
	wxSize Element::GetScrollRange() const
	{
		RECT range = {};
		if (::SciterGetScrollInfo(ToElement(m_Handle), nullptr, &range, nullptr) == SCDOM_OK)
		{
			return KxUtility::CopyRECTToRect(range).GetSize();
		}
		return wxDefaultSize;
	}
	bool Element::SetScrollPos(const wxPoint& pos) const
	{
		const Host* host = GetHost();
		return ::SciterSetScrollPos(ToElement(m_Handle), {pos.x, pos.y}, host ? host->IsSmoothScrollingEnabled() : false) == SCDOM_OK;
	}

	wxString Element::GetInnerHTML() const
	{
		wxString result;
		if (::SciterGetElementHtmlCB(ToElement(m_Handle), FALSE, ExtractWxString, &result) == SCDOM_OK)
		{
			return result;
		}
		return {};
	}
	wxString Element::GetOuterHTML() const
	{
		wxString result;
		if (::SciterGetElementHtmlCB(ToElement(m_Handle), TRUE, ExtractWxString, &result) == SCDOM_OK)
		{
			return result;
		}
		return {};
	}
	bool Element::SetInnerHTML(const wxString& html, ElementInnerHTML mode)
	{
		auto MapMode = [&]() -> std::optional<UINT>
		{
			switch (mode)
			{
				case ElementInnerHTML::Replace:
				{
					return SIH_REPLACE_CONTENT;
				}
				case ElementInnerHTML::InsertBefore:
				{
					return SIH_INSERT_AT_START;
				}
				case ElementInnerHTML::InsertAfter:
				{
					return SIH_APPEND_AFTER_LAST;
				}
			};
			return std::nullopt;
		};

		if (auto nativeMode = MapMode())
		{
			auto utf8 = html.ToUTF8();
			return ::SciterSetElementHtml(ToElement(m_Handle), reinterpret_cast<const BYTE*>(utf8.data()), utf8.length(), *nativeMode) == SCDOM_OK;
		}
		return false;
	}
	bool Element::SetOuterHTML(const wxString& html, ElementOuterHTML mode)
	{
		auto MapMode = [&]() -> std::optional<UINT>
		{
			switch (mode)
			{
				case ElementOuterHTML::Replace:
				{
					return SOH_REPLACE;
				}
				case ElementOuterHTML::InsertBefore:
				{
					return SOH_INSERT_BEFORE;
				}
				case ElementOuterHTML::InsertAfter:
				{
					return SOH_INSERT_AFTER;
				}
			};
			return std::nullopt;
		};

		if (auto nativeMode = MapMode())
		{
			auto utf8 = html.ToUTF8();
			return ::SciterSetElementHtml(ToElement(m_Handle), reinterpret_cast<const BYTE*>(utf8.data()), utf8.length(), *nativeMode) == SCDOM_OK;
		}
		return false;
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

	wxWindow* Element::GetWindow() const
	{
		if (HWND handle = GetNativeWindow())
		{
			for (auto node = wxTopLevelWindows.GetFirst(); node; node = node->GetNext())
			{
				wxWindow* window = node->GetData();
				if (window->GetHandle() == handle)
				{
					return window;
				}
				else if (wxWindow* childWindow = window->FindItemByHWND(handle))
				{
					return childWindow;
				}
			}
		}
		return nullptr;
	}
	HWND Element::GetNativeWindow() const
	{
		HWND windowHandle = nullptr;
		::SciterGetElementHwnd(ToElement(m_Handle), &windowHandle, FALSE);
		return windowHandle;
	}
	
	bool Element::AttachWindow(wxWindow& window)
	{
		return AttachNativeWindow(window.GetHandle());
	}
	bool Element::AttachNativeWindow(HWND handle)
	{
		if (handle)
		{
			return ::SciterAttachHwndToElement(ToElement(m_Handle), reinterpret_cast<HWND>(handle)) == SCDOM_OK;
		}
		return false;
	}
	
	wxWindow* Element::DetachWindow()
	{
		wxWindow* window = GetWindow();
		AttachNativeWindow(nullptr);

		return window;
	}
	HWND Element::DetachNativeWindow()
	{
		HWND window = GetNativeWindow();
		AttachNativeWindow(nullptr);

		return window;
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

	size_t Element::GetAttributeCount() const
	{
		UINT count = 0;
		::SciterGetAttributeCount(ToElement(m_Handle), &count);
		return count;
	}
	wxString Element::GetAttributeNameAt(size_t index) const
	{
		wxString result;
		::SciterGetNthAttributeNameCB(ToElement(m_Handle), index, ExtractWxString, &result);
		return result;
	}
	wxString Element::GetAttributeValueAt(size_t index) const
	{
		wxString result;
		::SciterGetNthAttributeValueCB(ToElement(m_Handle), index, ExtractWxString, &result);
		return result;
	}
	wxString Element::GetAttribute(const wxString& name) const
	{
		wxString result;

		auto nameUTF8 = name.ToUTF8();
		::SciterGetAttributeByNameCB(ToElement(m_Handle), nameUTF8.data(), ExtractWxString, &result);
		return result;
	}
	bool Element::SetAttribute(const wxString& name, const wxString& value)
	{
		auto nameUTF8 = name.ToUTF8();
		return ::SciterSetAttributeByName(ToElement(m_Handle), nameUTF8.data(), value.wc_str()) == SCDOM_OK;
	}
	bool Element::RemoveAttribute(const wxString& name)
	{
		auto nameUTF8 = name.ToUTF8();
		return ::SciterSetAttributeByName(ToElement(m_Handle), nameUTF8.data(), nullptr) == SCDOM_OK;
	}
	bool Element::ClearAttributes()
	{
		return ::SciterClearAttributes(ToElement(m_Handle)) == SCDOM_OK;
	}

	wxString Element::GetStyleAttribute(const wxString& name) const
	{
		wxString result;

		auto nameUTF8 = name.ToUTF8();
		::SciterGetStyleAttributeCB(ToElement(m_Handle), nameUTF8.data(), ExtractWxString, &result);
		return result;
	}
	KxColor Element::GetStyleAttributeColor(const wxString& name) const
	{
		return KxColor(GetStyleAttribute(name));
	}
	std::optional<int> Element::GetStyleAttributeInt(const wxString& name) const
	{
		long value = -1;
		if (GetStyleAttribute(name).ToCLong(&value))
		{
			return value;
		}
		return std::nullopt;
	}
	std::optional<double> Element::GetStyleAttributeFloat(const wxString& name) const
	{
		double value = -1;
		if (GetStyleAttribute(name).ToCDouble(&value))
		{
			return value;
		}
		return std::nullopt;
	}

	bool Element::SetStyleAttribute(const wxString& name, const wxString& value)
	{
		auto nameUTF8 = name.ToUTF8();
		return ::SciterSetStyleAttribute(ToElement(m_Handle), nameUTF8.data(), value.wc_str()) == SCDOM_OK;
	}
	bool Element::SetStyleAttribute(const wxString& name, const KxColor& value)
	{
		return SetStyleAttribute(name, value.ToString(KxColor::C2S::CSS, KxColor::C2SAlpha::Always));
	}
	bool Element::SetStyleAttribute(const wxString& name, int value, SizeUnit unit)
	{
		return SetStyleAttribute(name, KxString::Format(wxS("%1%2"), value, SizeUnitToString(unit)));
	}
	bool Element::SetStyleAttribute(const wxString& name, double value, SizeUnit unit)
	{
		return SetStyleAttribute(name, KxString::Format(wxS("%1%2"), value, SizeUnitToString(unit)));
	}
	bool Element::RemoveStyleAttribute(const wxString& name)
	{
		auto nameUTF8 = name.ToUTF8();
		return ::SciterSetStyleAttribute(ToElement(m_Handle), nameUTF8.data(), nullptr) == SCDOM_OK;
	}

	bool Element::SetStyleFont(const wxFont& font)
	{
		if (IsOk())
		{
			auto MapFamily = [&]() -> wxString
			{
				switch (font.GetFamily())
				{
					case wxFONTFAMILY_DECORATIVE:
					{
						return wxS("fantasy");
					}
					case wxFONTFAMILY_ROMAN:
					{
						return wxS("serif");
					}
					case wxFONTFAMILY_SWISS:
					{
						return wxS("sans-serif");
						break;
					}
					case wxFONTFAMILY_SCRIPT:
					{
						return wxS("cursive");
						break;
					}
					case wxFONTFAMILY_MODERN:
					case wxFONTFAMILY_TELETYPE:
					{
						return wxS("monospace");
						break;
					}
				};
				return {};
			};
			auto MapStyle = [&]() -> wxString
			{
				switch (font.GetStyle())
				{
					case wxFONTSTYLE_NORMAL:
					{
						return wxS("normal");
					}
					case wxFONTSTYLE_ITALIC:
					{
						return wxS("italic");
					}
					case wxFONTSTYLE_SLANT:
					{
						return wxS("oblique");
					}
				};
				return {};
			};

			// Family
			if (wxString family = MapFamily(); !family.IsEmpty())
			{
				SetStyleAttribute(wxS("font-family"), KxString::Format(wxS("\"%1\", %2"), font.GetFaceName(), family));
			}
			else
			{
				SetStyleAttribute(wxS("font-family"), KxString::Format(wxS("\"%1\""), font.GetFaceName()));
			}

			// Style
			if (wxString style = MapStyle(); !style.IsEmpty())
			{
				SetStyleAttribute(wxS("font-style"), style);
			}
			else
			{
				RemoveStyleAttribute(wxS("font-style"));
			}

			// Size
			SetStyleAttribute(wxS("font-size"), font.GetFractionalPointSize(), SizeUnit::pt);

			// Weight
			SetStyleAttribute(wxS("font-weight"), font.GetNumericWeight());

			return true;
		}
		return false;
	}
}
