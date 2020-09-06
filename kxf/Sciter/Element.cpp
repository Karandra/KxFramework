#include "stdafx.h"
#include "Element.h"
#include "Node.h"
#include "Host.h"
#include "EventDispatcher.h"
#include "ScriptValue.h"
#include "SciterAPI.h"
#include "Internal.h"
#include "kxf/General/RegEx.h"
#include "kxf/Utility/Drawing.h"

#pragma warning(disable: 4312) // 'reinterpret_cast': conversion from 'UINT' to 'void *' of greater size

namespace
{
	void ExtractInt(kxf::String& value)
	{
		for (size_t i = 0; i < value.length(); i++)
		{
			if (!std::isdigit(value[i]))
			{
				value.Truncate(i);
				break;
			}
		}
	}
	void ExtractFloat(kxf::String& value)
	{
		for (size_t i = 0; i < value.length(); i++)
		{
			auto c = value[i];
			if (!std::isdigit(c) && c != wxS('.'))
			{
				value.Truncate(i);
				break;
			}
		}
	}

	template<class TFunc>
	kxf::Sciter::Element DoGetElemenet(kxf::Sciter::ElementHandle* handle, TFunc&& func)
	{
		using namespace kxf::Sciter;

		HELEMENT node = nullptr;
		if (func(ToSciterElement(handle), &node) == SCDOM_OK)
		{
			return FromSciterElement(node);
		}
		return {};
	}

	bool DoCheckStateFlag(kxf::Sciter::ElementHandle* handle, ELEMENT_STATE_BITS flag)
	{
		using namespace kxf::Sciter;

		UINT state = 0;
		GetSciterAPI()->SciterGetElementState(ToSciterElement(handle), &state);

		return state & flag;
	}

	template<class TName>
	std::optional<int> DoGetStyleAttributeInt(const kxf::Sciter::Element& element, TName&& name)
	{
		kxf::String value = element.GetStyleAttribute(name);
		ExtractInt(value);

		return value.ToInt<int>();
	}

	template<class TName>
	std::optional<double> DoGetStyleAttributeFloat(const kxf::Sciter::Element& element, TName&& name)
	{
		kxf::String value = element.GetStyleAttribute(name);
		ExtractFloat(value);
		return value.ToFloatingPoint<double>();
	}

	void SC_CALLBACK ExtractStringLength(const wchar_t* value, UINT length, void* context)
	{
		*reinterpret_cast<size_t*>(context) = length;
	}
	void SC_CALLBACK ExtractStringLength(const char* value, UINT length, void* context)
	{
		*reinterpret_cast<size_t*>(context) = length;
	}
	void SC_CALLBACK ExtractStringLength(const BYTE* value, UINT length, void* context)
	{
		*reinterpret_cast<size_t*>(context) = length;
	}

	void SC_CALLBACK ExtractKxfString(const wchar_t* value, UINT length, void* context)
	{
		using namespace kxf;

		*reinterpret_cast<String*>(context) = String(value, length);
	}
	void SC_CALLBACK ExtractKxfString(const char* value, UINT length, void* context)
	{
		using namespace kxf;

		*reinterpret_cast<String*>(context) = String::FromUTF8(value, length);
	}
	void SC_CALLBACK ExtractKxfString(const BYTE* value, UINT length, void* context)
	{
		ExtractKxfString(reinterpret_cast<const char*>(value), length, context);
	}
}

namespace kxf::Sciter
{
	Element Element::Create(const String& tagName, const String& value)
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

	bool Element::DoAcquire(ElementHandle* handle) noexcept
	{
		return GetSciterAPI()->Sciter_UseElement(ToSciterElement(handle)) == SCDOM_OK;
	}
	void Element::DoRelease() noexcept
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
	void Element::AttachEventHandler(EvtHandler& evtHandler)
	{
		if (Host* host = GetHost())
		{
			host->AttachElementHandler(*this, evtHandler);
		}
	}
	void Element::DetachEventHandler(EvtHandler& evtHandler)
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
	bool Element::UpdateRect(const Rect& rect)
	{
		return GetSciterAPI()->SciterRefreshElementArea(ToSciterElement(m_Handle), Utility::ToWindowsRect(rect)) == SCDOM_OK;
	}

	// Size and position
	Rect Element::GetRect() const
	{
		RECT nativeRect = {};
		if (GetSciterAPI()->SciterGetElementLocation(ToSciterElement(m_Handle), &nativeRect, ELEMENT_AREAS::VIEW_RELATIVE) == SCDOM_OK)
		{
			return Utility::FromWindowsRect(nativeRect);
		}
		return {};
	}
	Point Element::GetPosition() const
	{
		return GetRect().GetPosition();
	}
	Size Element::GetSize() const
	{
		return GetRect().GetSize();
	}

	Size Element::GetMinSize() const
	{
		INT minWidth = 0;
		INT maxWidth = 0;
		INT height = 0;
		GetSciterAPI()->SciterGetElementIntrinsicWidths(ToSciterElement(m_Handle), &minWidth, &maxWidth);
		GetSciterAPI()->SciterGetElementIntrinsicHeight(ToSciterElement(m_Handle), minWidth, &height);

		return Size(minWidth, height);
	}
	Size Element::GetMaxSize() const
	{
		INT minWidth = 0;
		INT maxWidth = 0;
		INT height = 0;
		GetSciterAPI()->SciterGetElementIntrinsicWidths(ToSciterElement(m_Handle), &minWidth, &maxWidth);
		GetSciterAPI()->SciterGetElementIntrinsicHeight(ToSciterElement(m_Handle), maxWidth, &height);

		return Size(maxWidth, height);
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
	Point Element::GetScrollPos() const
	{
		POINT pos = {};
		if (GetSciterAPI()->SciterGetScrollInfo(ToSciterElement(m_Handle), &pos, nullptr, nullptr) == SCDOM_OK)
		{
			return Point(pos.x, pos.y);
		}
		return Point::UnspecifiedPosition();
	}
	Size Element::GetScrollRange() const
	{
		RECT range = {};
		if (GetSciterAPI()->SciterGetScrollInfo(ToSciterElement(m_Handle), nullptr, &range, nullptr) == SCDOM_OK)
		{
			return Utility::FromWindowsRect(range).GetSize();
		}
		return Size::UnspecifiedSize();
	}
	bool Element::SetScrollPos(const Point& pos) const
	{
		const Host* host = GetHost();
		return GetSciterAPI()->SciterSetScrollPos(ToSciterElement(m_Handle), {pos.GetX(), pos.GetY()}, host ? host->IsSmoothScrollingEnabled() : false) == SCDOM_OK;
	}

	// HTML content
	String Element::GetInnerHTML() const
	{
		String result;
		if (GetSciterAPI()->SciterGetElementHtmlCB(ToSciterElement(m_Handle), FALSE, ExtractKxfString, &result) == SCDOM_OK)
		{
			return result;
		}
		return {};
	}
	String Element::GetOuterHTML() const
	{
		String result;
		if (GetSciterAPI()->SciterGetElementHtmlCB(ToSciterElement(m_Handle), TRUE, ExtractKxfString, &result) == SCDOM_OK)
		{
			return result;
		}
		return {};
	}
	bool Element::SetInnerHTML(const String& html, ElementInnerHTML mode)
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
	bool Element::SetOuterHTML(const String& html, ElementOuterHTML mode)
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

	String Element::GetTagName() const
	{
		String result;
		GetSciterAPI()->SciterGetElementTypeCB(ToSciterElement(m_Handle), ExtractKxfString, &result);
		return result;
	}
	bool Element::SetTagName(const String& tagName)
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

			std::vector<std::pair<String, String>> attributes;
			attributes.reserve(attributeCount);
			for (size_t i = 0; i < attributeCount; i++)
			{
				attributes.emplace_back(GetAttributeNameAt(i), GetAttributeValueAt(i));
			}

			// Create empty element with required tag name
			Element newElement = Create(tagName);

			// Insert new the element into the DOM
			parent.InsertChildAt(newElement, GetIndexWithinParent());

			// Move children into new element
			for (Element& element: children)
			{
				newElement.AppendChild(element);
			}

			// Add text node if needed
			if (String value = GetValue(); !value.IsEmpty())
			{
				newElement.ToNode().AppendChild(Node::CreateTextNode(value));
			}

			// Add attributes
			for (const auto& [name, value]: attributes)
			{
				newElement.SetAttribute(name, value);
			}

			// Remove the original one
			Remove();

			// Now new element is this element
			*this = std::move(newElement);

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
	bool Element::AppendChild(const Element& childNode)
	{
		return InsertChildAt(childNode, GetIndexWithinParent() + 1);
	}
	bool Element::PrependChild(const Element& childNode)
	{
		const size_t index = GetIndexWithinParent();
		return InsertChildAt(childNode, index != 0 ? index - 1 : 0);
	}
	bool Element::InsertChildAt(const Element& childNode, size_t index)
	{
		return GetSciterAPI()->SciterInsertElement(ToSciterElement(childNode.m_Handle), ToSciterElement(m_Handle), index) == SCDOM_OK;
	}
	bool Element::InsertChildBefore(const Element& childNode)
	{
		const size_t index = childNode.GetIndexWithinParent();
		return InsertChildAt(childNode, index != 0 ? index - 1 : 0);
	}
	bool Element::InsertChildAfter(const Element& childNode)
	{
		const size_t index = childNode.GetIndexWithinParent();
		return InsertChildAt(childNode, index + 1);
	}

	// Native window
	wxWindow* Element::GetWindow() const
	{
		if (void* handle = GetNativeWindow())
		{
			for (wxWindow* window: wxTopLevelWindows)
			{
				if (window->GetHandle() == handle)
				{
					return window;
				}
				else if (wxWindow* childWindow = window->FindItemByHWND(static_cast<HWND>(handle)))
				{
					return childWindow;
				}
			}
		}
		return nullptr;
	}
	void* Element::GetNativeWindow() const
	{
		HWND windowHandle = nullptr;
		GetSciterAPI()->SciterGetElementHwnd(ToSciterElement(m_Handle), &windowHandle, FALSE);
		return windowHandle;
	}
	
	bool Element::AttachWindow(wxWindow& window)
	{
		return AttachNativeWindow(window.GetHandle());
	}
	bool Element::AttachNativeWindow(void* handle)
	{
		if (handle)
		{
			return GetSciterAPI()->SciterAttachHwndToElement(ToSciterElement(m_Handle), static_cast<HWND>(handle)) == SCDOM_OK;
		}
		return false;
	}
	
	wxWindow* Element::DetachWindow()
	{
		wxWindow* window = GetWindow();
		AttachNativeWindow(nullptr);

		return window;
	}
	void* Element::DetachNativeWindow()
	{
		void* window = GetNativeWindow();
		AttachNativeWindow(nullptr);

		return window;
	}

	// Text
	String Element::GetText() const
	{
		String result;
		GetSciterAPI()->SciterGetElementTextCB(ToSciterElement(m_Handle), ExtractKxfString, &result);
		return result;
	}
	bool Element::SetText(StringView text) const
	{
		return GetSciterAPI()->SciterSetElementText(ToSciterElement(m_Handle), text.data(), text.length()) == SCDOM_OK;
	}

	// Value
	String Element::GetValue() const
	{
		ScriptValue scriptValue;
		if (GetSciterAPI()->SciterGetValue(ToSciterElement(m_Handle), ToSciterScriptValue(scriptValue.GetNativeValue())) == SCDOM_OK)
		{
			return scriptValue.GetString();
		}
		return {};
	}
	bool Element::SetValue(StringView value) const
	{
		ScriptValue scriptValue = value;
		return GetSciterAPI()->SciterSetValue(ToSciterElement(m_Handle), ToSciterScriptValue(scriptValue.GetNativeValue())) == SCDOM_OK;
	}

	// Attributes
	bool Element::HasAttribute(const String& name) const
	{
		auto nameUTF8 = name.ToUTF8();
		return HasAttribute(nameUTF8.data());
	}
	bool Element::HasAttribute(const char* name) const
	{
		size_t length = 0;
		return GetSciterAPI()->SciterGetAttributeByNameCB(ToSciterElement(m_Handle), name, ExtractStringLength, &length);
		return length != 0;
	}

	String Element::GetAttribute(const String& name) const
	{
		auto nameUTF8 = name.ToUTF8();
		return GetAttribute(nameUTF8.data());
	}
	String Element::GetAttribute(const char* name) const
	{
		String result;

		GetSciterAPI()->SciterGetAttributeByNameCB(ToSciterElement(m_Handle), name, ExtractKxfString, &result);
		return result;
	}

	size_t Element::GetAttributeCount() const
	{
		UINT count = 0;
		GetSciterAPI()->SciterGetAttributeCount(ToSciterElement(m_Handle), &count);
		return count;
	}
	String Element::GetAttributeNameAt(size_t index) const
	{
		String result;
		GetSciterAPI()->SciterGetNthAttributeNameCB(ToSciterElement(m_Handle), index, ExtractKxfString, &result);
		return result;
	}
	String Element::GetAttributeValueAt(size_t index) const
	{
		String result;
		GetSciterAPI()->SciterGetNthAttributeValueCB(ToSciterElement(m_Handle), index, ExtractKxfString, &result);
		return result;
	}

	bool Element::SetAttribute(const String& name, const String& value)
	{
		auto nameUTF8 = name.ToUTF8();
		return SetAttribute(nameUTF8.data(), value);
	}
	bool Element::SetAttribute(const char* name, const String& value)
	{
		return GetSciterAPI()->SciterSetAttributeByName(ToSciterElement(m_Handle), name, value.wc_str()) == SCDOM_OK;
	}
	bool Element::RemoveAttribute(const String& name)
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
	bool Element::HasStyleAttribute(const String& name) const
	{
		auto nameUTF8 = name.ToUTF8();
		return HasStyleAttribute(nameUTF8.data());
	}
	bool Element::HasStyleAttribute(const char* name) const
	{
		size_t length = 0;
		return GetSciterAPI()->SciterGetStyleAttributeCB(ToSciterElement(m_Handle), name, ExtractStringLength, &length);
		return length != 0;
	}

	String Element::GetStyleAttribute(const String& name) const
	{
		auto nameUTF8 = name.ToUTF8();
		return GetStyleAttribute(nameUTF8.data());
	}
	String Element::GetStyleAttribute(const char* name) const
	{
		String result;
		GetSciterAPI()->SciterGetStyleAttributeCB(ToSciterElement(m_Handle), name, ExtractKxfString, &result);
		return result;
	}

	std::optional<int> Element::GetStyleAttributeInt(const char* name) const
	{
		return DoGetStyleAttributeInt(*this, name);
	}
	std::optional<int> Element::GetStyleAttributeInt(const String& name) const
	{
		return DoGetStyleAttributeInt(*this, name);
	}

	std::optional<double> Element::GetStyleAttributeFloat(const String& name) const
	{
		return DoGetStyleAttributeFloat(*this, name);
	}
	std::optional<double> Element::GetStyleAttributeFloat(const char* name) const
	{
		return DoGetStyleAttributeFloat(*this, name);
	}

	Color Element::GetStyleAttributeColor(const char* name, ColorSpace* colorSpace) const
	{
		return Color::FromString(GetStyleAttribute(name), colorSpace);
	}
	Color Element::GetStyleAttributeColor(const String& name, ColorSpace* colorSpace) const
	{
		return Color::FromString(GetStyleAttribute(name), colorSpace);
	}

	bool Element::SetStyleAttribute(const String& name, const String& value)
	{
		auto nameUTF8 = name.ToUTF8();
		return SetStyleAttribute(nameUTF8.data(), value);
	}
	bool Element::SetStyleAttribute(const char* name, const String& value)
	{
		return GetSciterAPI()->SciterSetStyleAttribute(ToSciterElement(m_Handle), name, value.wc_str()) == SCDOM_OK;
	}

	bool Element::SetStyleAttribute(const String& name, const Color& value)
	{
		return SetStyleAttribute(name, value.ToString(C2SFormat::CSS, C2SAlpha::Always));
	}
	bool Element::SetStyleAttribute(const char* name, const Color& value)
	{
		return SetStyleAttribute(name, value.ToString(C2SFormat::CSS, C2SAlpha::Always));
	}

	bool Element::SetStyleAttribute(const String& name, int value, SizeUnit unit)
	{
		return SetStyleAttribute(name, String::Format(wxS("%1%2"), value, SizeUnitToString(unit)));
	}
	bool Element::SetStyleAttribute(const char* name, int value, SizeUnit unit)
	{
		return SetStyleAttribute(name, String::Format(wxS("%1%2"), value, SizeUnitToString(unit)));
	}

	bool Element::SetStyleAttribute(const String& name, double value, SizeUnit unit)
	{
		return SetStyleAttribute(name, String::Format(wxS("%1%2"), value, SizeUnitToString(unit)));
	}
	bool Element::SetStyleAttribute(const char* name, double value, SizeUnit unit)
	{
		return SetStyleAttribute(name, String::Format(wxS("%1%2"), value, SizeUnitToString(unit)));
	}

	bool Element::RemoveStyleAttribute(const String& name)
	{
		auto nameUTF8 = name.ToUTF8();
		return RemoveStyleAttribute(nameUTF8.data());
	}
	bool Element::RemoveStyleAttribute(const char* name)
	{
		return GetSciterAPI()->SciterSetStyleAttribute(ToSciterElement(m_Handle), name, nullptr) == SCDOM_OK;
	}

	wxFont Element::GetStyleFont() const
	{
		wxFont font;

		// Family
		if (String fontFamily = GetStyleAttribute("font-family"); !fontFamily.IsEmpty())
		{
			if (RegEx regEx(wxS(R"("?([\w\s]+)\"?)")); regEx.Matches(fontFamily))
			{
				for (size_t i = 1; i <= regEx.GetMatchCount(); i++)
				{
					String value = regEx.GetMatch(fontFamily, i).Trim().Trim(StringOpFlag::FromEnd);
					if (i == regEx.GetMatchCount())
					{
						if (value == wxS("fantasy"))
						{
							font.SetFamily(wxFONTFAMILY_DECORATIVE);
						}
						else if (value == wxS("serif"))
						{
							font.SetFamily(wxFONTFAMILY_ROMAN);
						}
						else if (value == wxS("sans-serif"))
						{
							font.SetFamily(wxFONTFAMILY_SWISS);
						}
						else if (value == wxS("cursive"))
						{
							font.SetFamily(wxFONTFAMILY_SCRIPT);
						}
						else if (value == wxS("monospace"))
						{
							// Or maybe 'wxFONTFAMILY_MODERN'
							font.SetFamily(wxFONTFAMILY_TELETYPE);
						}
					}
					if (font.SetFaceName(value))
					{
						break;
					}
				}
			}
		}

		// Style
		if (String fontStyle = GetStyleAttribute("font-style"); !fontStyle.IsEmpty())
		{
			if (fontStyle == wxS("normal"))
			{
				font.SetStyle(wxFONTSTYLE_NORMAL);
			}
			else if (fontStyle == wxS("italic"))
			{
				font.SetStyle(wxFONTSTYLE_ITALIC);
			}
			else if (fontStyle == wxS("oblique"))
			{
				font.SetStyle(wxFONTSTYLE_SLANT);
			}
		}

		// Size
		if (auto fontSize = GetStyleAttributeFloat("font-size"))
		{
			font.SetFractionalPointSize(*fontSize);
		}

		// Weight
		if (auto fontWeight = GetStyleAttributeFloat("font-weight"))
		{
			font.SetNumericWeight(*fontWeight);
		}

		return font;
	}
	bool Element::SetStyleFont(const wxFont& font)
	{
		if (!IsNull())
		{
			auto MapFamily = [&]() -> String
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
			auto MapStyle = [&]() -> String
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
			if (String family = MapFamily(); !family.IsEmpty())
			{
				SetStyleAttribute("font-family", String::Format(wxS(R"("%1", %2)"), font.GetFaceName(), family));
			}
			else
			{
				SetStyleAttribute("font-family", String::Format(wxS(R"("%1")"), font.GetFaceName()));
			}

			// Style
			if (String style = MapStyle(); !style.IsEmpty())
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
	size_t Element::Select(const String& query, TOnElement onElement) const
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
	Element Element::SelectAny(const String& query) const
	{
		Element result;
		Select(query, [&result](Element element)
		{
			result = std::move(element);
			return false;
		});
		return result;
	}
	std::vector<Element> Element::SelectAll(const String& query) const
	{
		std::vector<Element> results;
		Select(query, [&results](Element element)
		{
			results.emplace_back(std::move(element));
			return true;
		});
		return results;
	}

	ScriptValue Element::ExecuteScript(const String& script)
	{
		ScriptValue result;
		GetSciterAPI()->SciterEvalElementScript(ToSciterElement(m_Handle), script.wc_str(), script.length(), ToSciterScriptValue(result.GetNativeValue()));
		return result;
	}
}
