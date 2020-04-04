#include "KxStdAfx.h"
#include "Element.h"
#include "Node.h"
#include "Host.h"
#include "EventDispatcher.h"
#include "SciterAPI.h"
#include "Internal.h"
#include "KxFramework/KxUtility.h"

#pragma warning(disable: 4312) // 'reinterpret_cast': conversion from 'UINT' to 'void *' of greater size

namespace KxFramework::Sciter
{
	template<class TFunc>
	Element DoGetElemenet(ElementHandle* handle, TFunc&& func)
	{
		HELEMENT node = nullptr;
		if (func(ToSciterElement(handle), &node) == SCDOM_OK)
		{
			return FromSciterElement(node);
		}
		return {};
	}

	bool DoCheckStateFlag(ElementHandle* handle, ELEMENT_STATE_BITS flag)
	{
		UINT state = 0;
		GetSciterAPI()->SciterGetElementState(ToSciterElement(handle), &state);

		return state & flag;
	}

	template<class TName>
	std::optional<int> DoGetStyleAttributeInt(const Element& element, TName&& name)
	{
		wxString value = element.GetStyleAttribute(name);
		if (long iValue = -1; value.ToCLong(&iValue))
		{
			return iValue;
		}
		return std::nullopt;
	}

	template<class TName>
	std::optional<double> DoGetStyleAttributeFloat(const Element& element, TName&& name)
	{
		wxString value = element.GetStyleAttribute(name);
		if (double fValue = -1; value.ToCDouble(&fValue))
		{
			return fValue;
		}
		return std::nullopt;
	}

	void __stdcall ExtractWxString(const wchar_t* value, UINT length, void* context)
	{
		reinterpret_cast<wxString*>(context)->assign(value, length);
	}
	void __stdcall ExtractWxString(const char* value, UINT length, void* context)
	{
		*reinterpret_cast<wxString*>(context) = wxString::FromUTF8(value, length);
	}
	void __stdcall ExtractWxString(const BYTE* value, UINT length, void* context)
	{
		ExtractWxString(reinterpret_cast<const char*>(value), length, context);
	}
}

namespace KxFramework::Sciter
{
	Element Element::Create(const wxString& tagName, const wxString& value)
	{
		Element node;

		auto tagNameUTF8 = tagName.ToUTF8();
		HELEMENT nativeNode = nullptr;
		if (GetSciterAPI()->SciterCreateElement(tagNameUTF8.data(), value.wc_str(), &nativeNode) == SCDOM_OK)
		{
			node.AttachHandle(FromSciterElement(nativeNode));
		}
		return node;
	}

	bool Element::DoAcquire(ElementHandle* handle)
	{
		return GetSciterAPI()->Sciter_UseElement(ToSciterElement(handle)) == SCDOM_OK;
	}
	void Element::DoRelease()
	{
		GetSciterAPI()->Sciter_UnuseElement(ToSciterElement(m_Handle));
	}

	ElementUID* Element::GetUID() const
	{
		UINT id = 0;
		GetSciterAPI()->SciterGetElementUID(ToSciterElement(m_Handle), &id);
		return reinterpret_cast<ElementUID*>(id);
	}
	Host* Element::GetHost() const
	{
		HWND windowHandle = nullptr;
		GetSciterAPI()->SciterGetElementHwnd(ToSciterElement(m_Handle), &windowHandle, TRUE);
		if (windowHandle)
		{
			return reinterpret_cast<Host*>(::GetWindowLongPtrW(windowHandle, GWLP_USERDATA));
		}
		return nullptr;
	}

	bool Element::Detach()
	{
		return GetSciterAPI()->SciterDetachElement(ToSciterElement(m_Handle)) == SCDOM_OK;
	}
	bool Element::Remove()
	{
		if (GetSciterAPI()->SciterDeleteElement(ToSciterElement(m_Handle)) == SCDOM_OK)
		{
			Release();
			return true;
		}
		return false;
	}

	Node Element::ToNode() const
	{
		HNODE node = nullptr;
		GetSciterAPI()->SciterNodeCastFromElement(ToSciterElement(m_Handle), &node);
		return Node(reinterpret_cast<NodeHandle*>(node));
	}
	Element Element::Clone() const
	{
		HELEMENT nativeNode = nullptr;
		if (GetSciterAPI()->SciterCloneElement(ToSciterElement(m_Handle), &nativeNode) == SCDOM_OK)
		{
			Element node;
			node.AttachHandle(FromSciterElement(nativeNode));
			return node;
		}
		return {};
	}
	void Element::Swap(Element& other)
	{
		GetSciterAPI()->SciterSwapElements(ToSciterElement(m_Handle), ToSciterElement(other.m_Handle));
	}

	// Event handling
	void Element::AttachEventHandler()
	{
		if (Host* host = GetHost())
		{
			host->AttachElementHandler(*this);
		}
	}
	void Element::DetachEventHandler()
	{
		if (Host* host = GetHost())
		{
			host->DetachElementHandler(*this);
		}
	}
	void Element::AttachEventHandler(wxEvtHandler& evtHandler)
	{
		if (Host* host = GetHost())
		{
			host->AttachElementHandler(*this, evtHandler);
		}
	}
	void Element::DetachEventHandler(wxEvtHandler& evtHandler)
	{
		if (Host* host = GetHost())
		{
			host->DetachElementHandler(*this, evtHandler);
		}
	}

	// Refreshing
	bool Element::Update(bool force)
	{
		return GetSciterAPI()->SciterUpdateElement(ToSciterElement(m_Handle), force) == SCDOM_OK;
	}
	bool Element::UpdateRect(const wxRect& rect)
	{
		return GetSciterAPI()->SciterRefreshElementArea(ToSciterElement(m_Handle), KxUtility::CopyRectToRECT(rect)) == SCDOM_OK;
	}

	// Size and position
	wxRect Element::GetRect() const
	{
		RECT nativeRect = {};
		if (GetSciterAPI()->SciterGetElementLocation(ToSciterElement(m_Handle), &nativeRect, ELEMENT_AREAS::VIEW_RELATIVE) == SCDOM_OK)
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
		GetSciterAPI()->SciterGetElementIntrinsicWidths(ToSciterElement(m_Handle), &minWidth, &maxWidth);
		GetSciterAPI()->SciterGetElementIntrinsicHeight(ToSciterElement(m_Handle), minWidth, &height);

		return wxSize(minWidth, height);
	}
	wxSize Element::GetMaxSize() const
	{
		INT minWidth = 0;
		INT maxWidth = 0;
		INT height = 0;
		GetSciterAPI()->SciterGetElementIntrinsicWidths(ToSciterElement(m_Handle), &minWidth, &maxWidth);
		GetSciterAPI()->SciterGetElementIntrinsicHeight(ToSciterElement(m_Handle), maxWidth, &height);

		return wxSize(maxWidth, height);
	}

	// Visibility
	bool Element::IsVisible() const
	{
		BOOL result = FALSE;
		GetSciterAPI()->SciterIsElementVisible(ToSciterElement(m_Handle), &result);

		return result;
	}
	void Element::SetVisible(bool visible)
	{
		if (visible)
		{
			SetStyleAttribute("visibility", "hidden");
		}
		else
		{
			SetStyleAttribute("visibility", "inherit");
		}
	}

	// Misc
	bool Element::SetCapture()
	{
		return GetSciterAPI()->SciterSetCapture(ToSciterElement(m_Handle)) == SCDOM_OK;
	}
	bool Element::ReleaseCapture()
	{
		return GetSciterAPI()->SciterReleaseCapture(ToSciterElement(m_Handle)) == SCDOM_OK;
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
		GetSciterAPI()->SciterSetElementState(ToSciterElement(m_Handle), ELEMENT_STATE_BITS::STATE_FOCUS, 0, FALSE);
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

	// Scrolling
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

		GetSciterAPI()->SciterScrollToView(ToSciterElement(m_Handle), nativeFlags);
	}
	wxPoint Element::GetScrollPos() const
	{
		POINT pos = {};
		if (GetSciterAPI()->SciterGetScrollInfo(ToSciterElement(m_Handle), &pos, nullptr, nullptr) == SCDOM_OK)
		{
			return wxPoint(pos.x, pos.y);
		}
		return wxDefaultPosition;
	}
	wxSize Element::GetScrollRange() const
	{
		RECT range = {};
		if (GetSciterAPI()->SciterGetScrollInfo(ToSciterElement(m_Handle), nullptr, &range, nullptr) == SCDOM_OK)
		{
			return KxUtility::CopyRECTToRect(range).GetSize();
		}
		return wxDefaultSize;
	}
	bool Element::SetScrollPos(const wxPoint& pos) const
	{
		const Host* host = GetHost();
		return GetSciterAPI()->SciterSetScrollPos(ToSciterElement(m_Handle), {pos.x, pos.y}, host ? host->IsSmoothScrollingEnabled() : false) == SCDOM_OK;
	}

	// HTML content
	wxString Element::GetInnerHTML() const
	{
		wxString result;
		if (GetSciterAPI()->SciterGetElementHtmlCB(ToSciterElement(m_Handle), FALSE, ExtractWxString, &result) == SCDOM_OK)
		{
			return result;
		}
		return {};
	}
	wxString Element::GetOuterHTML() const
	{
		wxString result;
		if (GetSciterAPI()->SciterGetElementHtmlCB(ToSciterElement(m_Handle), TRUE, ExtractWxString, &result) == SCDOM_OK)
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
			auto utf8 = ToSciterUTF8(html);
			return GetSciterAPI()->SciterSetElementHtml(ToSciterElement(m_Handle), utf8.data(), utf8.size(), *nativeMode) == SCDOM_OK;
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
			auto utf8 = ToSciterUTF8(html);
			return GetSciterAPI()->SciterSetElementHtml(ToSciterElement(m_Handle), utf8.data(), utf8.size(), *nativeMode) == SCDOM_OK;
		}
		return false;
	}

	wxString Element::GetTagName() const
	{
		wxString result;
		GetSciterAPI()->SciterGetElementTypeCB(ToSciterElement(m_Handle), ExtractWxString, &result);
		return result;
	}
	bool Element::SetTagName(const wxString& tagName)
	{
		if (GetTagName() == tagName)
		{
			return true;
		}

		// I can't believe there isn't a function is Sciter to change element's tag name.
		// This ugly hack is absolutely terrible.
		if (Element parent = GetParent())
		{
			// Collect all element's children
			const size_t childrenCount = GetChildrenCount();

			std::vector<Element> children;
			children.reserve(childrenCount);
			for (size_t i = 0; i < childrenCount; i++)
			{
				children.emplace_back(GetChildAt(i));
			}

			// Collect all attributes
			const size_t attributeCount = GetAttributeCount();

			std::vector<std::pair<wxString, wxString>> attributes;
			attributes.reserve(attributeCount);
			for (size_t i = 0; i < attributeCount; i++)
			{
				attributes.emplace_back(GetAttributeNameAt(i), GetAttributeValueAt(i));
			}

			// Create empty element with required tag name
			Element newElement = Create(tagName);

			// Insert new the element into the DOM
			parent.InsertAt(newElement, GetIndexWithinParent());

			// Move children into new element
			for (Element& element: children)
			{
				newElement.Append(element);
			}

			// Add text node if needed
			if (wxString value = GetValue(); !value.IsEmpty())
			{
				newElement.ToNode().Append(Node::CreateTextNode(value));
			}

			// Add attributes
			for (const auto& [name, value]: attributes)
			{
				newElement.SetAttribute(name, value);
			}

			// Remove the original one
			Remove();

			// Now new element is this element
			*this = newElement;

			return true;
		}
		return false;
	}

	// Children and parents
	Element Element::GetRoot() const
	{
		if (Host* host = GetHost())
		{
			return host->GetRootElement();
		}
		return {};
	}
	Element Element::GetParent() const
	{
		return DoGetElemenet(m_Handle, GetSciterAPI()->SciterGetParentElement);
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
		GetSciterAPI()->SciterGetElementIndex(ToSciterElement(m_Handle), &index);
		return index;
	}
	size_t Element::GetChildrenCount() const
	{
		UINT count = 0;
		GetSciterAPI()->SciterGetChildrenCount(ToSciterElement(m_Handle), &count);

		return count;
	}
	Element Element::GetChildAt(size_t index) const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetNthChild(ToSciterElement(m_Handle), index, &node) == SCDOM_OK)
		{
			return FromSciterElement(node);
		}
		return {};
	}

	// Insertion
	bool Element::Append(const Element& node)
	{
		return InsertAt(node, GetIndexWithinParent() + 1);
	}
	bool Element::Prepend(const Element& node)
	{
		const size_t index = GetIndexWithinParent();
		return InsertAt(node, index != 0 ? index - 1 : 0);
	}
	bool Element::InsertAt(const Element& node, size_t index)
	{
		return GetSciterAPI()->SciterInsertElement(ToSciterElement(node.m_Handle), ToSciterElement(m_Handle), index) == SCDOM_OK;
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

	// Native window
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
		GetSciterAPI()->SciterGetElementHwnd(ToSciterElement(m_Handle), &windowHandle, FALSE);
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
			return GetSciterAPI()->SciterAttachHwndToElement(ToSciterElement(m_Handle), reinterpret_cast<HWND>(handle)) == SCDOM_OK;
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

	// Text
	wxString Element::GetText() const
	{
		wxString result;
		GetSciterAPI()->SciterGetElementTextCB(ToSciterElement(m_Handle), ExtractWxString, &result);
		return result;
	}
	bool Element::SetText(const wxString& text) const
	{
		return GetSciterAPI()->SciterSetElementText(ToSciterElement(m_Handle), text.wc_str(), text.length()) == SCDOM_OK;
	}

	// Value
	wxString Element::GetValue() const
	{
		VALUE value = {};
		if (GetSciterAPI()->SciterGetValue(ToSciterElement(m_Handle), &value) == SCDOM_OK && value.t == VALUE_TYPE::T_STRING)
		{
			aux::wchars slice;
			if (GetSciterAPI()->ValueStringData(&value, &slice.start, &slice.length) == HV_OK)
			{
				return wxString(slice.begin(), slice.end());
			}
		}
		return {};
	}
	bool Element::SetValue(const wxString& value) const
	{
		VALUE nativeValue = {};
		GetSciterAPI()->ValueInit(&nativeValue);
		GetSciterAPI()->ValueStringDataSet(&nativeValue, value.wc_str(), value.length(), T_STRING);

		return GetSciterAPI()->SciterSetValue(ToSciterElement(m_Handle), &nativeValue) == SCDOM_OK;
	}

	// Attributes
	size_t Element::GetAttributeCount() const
	{
		UINT count = 0;
		GetSciterAPI()->SciterGetAttributeCount(ToSciterElement(m_Handle), &count);
		return count;
	}
	wxString Element::GetAttributeNameAt(size_t index) const
	{
		wxString result;
		GetSciterAPI()->SciterGetNthAttributeNameCB(ToSciterElement(m_Handle), index, ExtractWxString, &result);
		return result;
	}
	wxString Element::GetAttributeValueAt(size_t index) const
	{
		wxString result;
		GetSciterAPI()->SciterGetNthAttributeValueCB(ToSciterElement(m_Handle), index, ExtractWxString, &result);
		return result;
	}
	wxString Element::GetAttribute(const wxString& name) const
	{
		auto nameUTF8 = name.ToUTF8();
		return GetAttribute(nameUTF8.data());
	}
	wxString Element::GetAttribute(const char* name) const
	{
		wxString result;

		GetSciterAPI()->SciterGetAttributeByNameCB(ToSciterElement(m_Handle), name, ExtractWxString, &result);
		return result;
	}

	bool Element::SetAttribute(const wxString& name, const wxString& value)
	{
		auto nameUTF8 = name.ToUTF8();
		return SetAttribute(nameUTF8.data(), value);
	}
	bool Element::SetAttribute(const char* name, const wxString& value)
	{
		return GetSciterAPI()->SciterSetAttributeByName(ToSciterElement(m_Handle), name, value.wc_str()) == SCDOM_OK;
	}
	bool Element::RemoveAttribute(const wxString& name)
	{
		auto nameUTF8 = name.ToUTF8();
		return RemoveAttribute(nameUTF8.data());
	}
	bool Element::RemoveAttribute(const char* name)
	{
		return GetSciterAPI()->SciterSetAttributeByName(ToSciterElement(m_Handle), name, nullptr) == SCDOM_OK;
	}
	bool Element::ClearAttributes()
	{
		return GetSciterAPI()->SciterClearAttributes(ToSciterElement(m_Handle)) == SCDOM_OK;
	}

	// Style (CSS) attributes
	wxString Element::GetStyleAttribute(const wxString& name) const
	{
		auto nameUTF8 = name.ToUTF8();
		return GetStyleAttribute(nameUTF8.data());
	}
	wxString Element::GetStyleAttribute(const char* name) const
	{
		wxString result;
		GetSciterAPI()->SciterGetStyleAttributeCB(ToSciterElement(m_Handle), name, ExtractWxString, &result);
		return result;
	}

	std::optional<int> Element::GetStyleAttributeInt(const char* name) const
	{
		return DoGetStyleAttributeInt(*this, name);
	}
	std::optional<int> Element::GetStyleAttributeInt(const wxString& name) const
	{
		return DoGetStyleAttributeInt(*this, name);
	}

	std::optional<double> Element::GetStyleAttributeFloat(const wxString& name) const
	{
		return DoGetStyleAttributeFloat(*this, name);
	}
	std::optional<double> Element::GetStyleAttributeFloat(const char* name) const
	{
		return DoGetStyleAttributeFloat(*this, name);
	}

	bool Element::SetStyleAttribute(const wxString& name, const wxString& value)
	{
		auto nameUTF8 = name.ToUTF8();
		return SetStyleAttribute(nameUTF8.data(), value);
	}
	bool Element::SetStyleAttribute(const char* name, const wxString& value)
	{
		return GetSciterAPI()->SciterSetStyleAttribute(ToSciterElement(m_Handle), name, value.wc_str()) == SCDOM_OK;
	}

	bool Element::SetStyleAttribute(const wxString& name, const Color& value)
	{
		return SetStyleAttribute(name, value.ToString(C2SFormat::CSS, C2SAlpha::Always));
	}
	bool Element::SetStyleAttribute(const char* name, const Color& value)
	{
		return SetStyleAttribute(name, value.ToString(C2SFormat::CSS, C2SAlpha::Always));
	}

	bool Element::SetStyleAttribute(const wxString& name, int value, SizeUnit unit)
	{
		return SetStyleAttribute(name, KxString::Format(wxS("%1%2"), value, SizeUnitToString(unit)));
	}
	bool Element::SetStyleAttribute(const char* name, int value, SizeUnit unit)
	{
		return SetStyleAttribute(name, KxString::Format(wxS("%1%2"), value, SizeUnitToString(unit)));
	}

	bool Element::SetStyleAttribute(const wxString& name, double value, SizeUnit unit)
	{
		return SetStyleAttribute(name, KxString::Format(wxS("%1%2"), value, SizeUnitToString(unit)));
	}
	bool Element::SetStyleAttribute(const char* name, double value, SizeUnit unit)
	{
		return SetStyleAttribute(name, KxString::Format(wxS("%1%2"), value, SizeUnitToString(unit)));
	}

	bool Element::RemoveStyleAttribute(const wxString& name)
	{
		auto nameUTF8 = name.ToUTF8();
		return RemoveStyleAttribute(nameUTF8.data());
	}
	bool Element::RemoveStyleAttribute(const char* name)
	{
		return GetSciterAPI()->SciterSetStyleAttribute(ToSciterElement(m_Handle), name, nullptr) == SCDOM_OK;
	}

	bool Element::SetStyleFont(const wxFont& font)
	{
		if (!IsNull())
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
				SetStyleAttribute("font-family", KxString::Format(wxS("\"%1\", %2"), font.GetFaceName(), family));
			}
			else
			{
				SetStyleAttribute("font-family", KxString::Format(wxS("\"%1\""), font.GetFaceName()));
			}

			// Style
			if (wxString style = MapStyle(); !style.IsEmpty())
			{
				SetStyleAttribute("font-style", style);
			}
			else
			{
				RemoveStyleAttribute("font-style");
			}

			// Size
			SetStyleAttribute("font-size", font.GetFractionalPointSize(), SizeUnit::pt);

			// Weight
			SetStyleAttribute("font-weight", font.GetNumericWeight());

			return true;
		}
		return false;
	}

	// Selectors
	size_t Element::Select(const wxString& query, TOnElement onElement) const
	{
		struct CallContext
		{
			TOnElement& Callback;
			size_t Count = 0;
		};

		CallContext context = {onElement};
		GetSciterAPI()->SciterSelectElementsW(ToSciterElement(m_Handle), query.wc_str(), [](HELEMENT nativeElement, void* context) -> BOOL
		{
			CallContext& callContext = *reinterpret_cast<CallContext*>(context);
			callContext.Count++;

			return !std::invoke(callContext.Callback, FromSciterElement(nativeElement));
		}, &context);
		return context.Count;
	}
	Element Element::SelectAny(const wxString& query) const
	{
		Element result;
		Select(query, [&result](Element element)
		{
			result = std::move(element);
			return false;
		});
		return result;
	}
	std::vector<Element> Element::SelectAll(const wxString& query) const
	{
		std::vector<Element> results;
		Select(query, [&results](Element element)
		{
			results.emplace_back(std::move(element));
			return true;
		});
		return results;
	}

	ScriptValue Element::ExecuteScript(const wxString& script)
	{
		ScriptValue result;
		GetSciterAPI()->SciterEvalElementScript(ToSciterElement(m_Handle), script.wc_str(), script.length(), ToSciterScriptValue(result.GetNativeValue()));
		return result;
	}
}
