#pragma once
#include "Common.h"
#include <KxFramework/KxColor.h>
#include <utility>

namespace KxSciter
{
	class Node;
	class Host;

	struct ElementHandle;
	struct ElementUID;
}

namespace KxSciter
{
	class KX_API Element final
	{
		public:
			using TOnElement = std::function<bool(Element)>;

		public:
			static Element Create(const wxString& tagName, const wxString& value = {});

		private:
			ElementHandle* m_Handle = nullptr;

		private:
			void Acquire(ElementHandle* handle);
			void Release();

			void CopyFrom(const Element& other);
			void CopyFrom(ElementHandle* handle)
			{
				Release();
				Acquire(handle);
			}
			void MoveFrom(Element& other);

		public:
			Element() = default;
			Element(ElementHandle* handle)
			{
				Acquire(handle);
			}
			Element(const Element& other)
			{
				CopyFrom(other);
			}
			Element(Element&& other)
			{
				MoveFrom(other);
			}
			~Element()
			{
				Release();
			}
			
		public:
			bool IsOk() const
			{
				return m_Handle != nullptr;
			}
			void MakeNull()
			{
				Release();
			}
			
			ElementHandle* GetHandle() const
			{
				return m_Handle;
			}
			ElementUID* GetUID() const;
			Host* GetHost() const;

			bool AttachHandle(ElementHandle* handle);
			ElementHandle* Detach();
			bool Remove();

			Node ToNode() const;
			Element Clone() const;
			void Swap(Element& other);

			// Event handling
			void AttachEventHandler();
			void DetachEventHandler();
			void AttachEventHandler(wxEvtHandler& evtHandler);
			void DetachEventHandler(wxEvtHandler& evtHandler);

			// Refreshing
			bool Update(bool force = false);
			bool UpdateRect(const wxRect& rect);

			// Size and position
			wxRect GetRect() const;
			wxPoint GetPosition() const;
			wxSize GetSize() const;

			wxSize GetMinSize() const;
			wxSize GetMaxSize() const;

			// Visibility
			bool IsVisible() const;
			void SetVisible(bool visible = true);

			// Misc
			bool SetCapture();
			bool ReleaseCapture();

			bool IsFocusable() const;
			bool HasFocus() const;
			void SetFocus();

			bool IsHighlighted() const;
			void SetHighlighted();

			// Scrolling
			void ScrollIntoView(bool toTop = false);
			wxPoint GetScrollPos() const;
			wxSize GetScrollRange() const;
			bool SetScrollPos(const wxPoint& pos) const;

			// HTML content
			wxString GetInnerHTML() const;
			wxString GetOuterHTML() const;
			bool SetInnerHTML(const wxString& html, ElementInnerHTML mode);
			bool SetOuterHTML(const wxString& html, ElementOuterHTML mode);

			wxString GetTagName() const;
			bool SetTagName(const wxString& tagName);

			// Children and parents
			Element GetRoot() const;
			Element GetParent() const;
			Element GetPrevSibling() const;
			Element GetNextSibling() const;
			Element GetFirstChild() const;
			Element GetLastChild() const;

			size_t GetIndexWithinParent() const;
			size_t GetChildrenCount() const;
			Element GetChildAt(size_t index) const;
			Element operator[](size_t index) const
			{
				return GetChildAt(index);
			}
			
			// Insertion
			bool Append(const Element& node);
			bool Prepend(const Element& node);
			bool InsertAt(const Element& node, size_t index);
			bool InsertBefore(const Element& node);
			bool InsertAfter(const Element& node);

			// Native window
			wxWindow* GetWindow() const;
			HWND GetNativeWindow() const;

			bool AttachWindow(wxWindow& window);
			bool AttachNativeWindow(HWND handle);

			wxWindow* DetachWindow();
			HWND DetachNativeWindow();

			// Text
			wxString GetText() const;
			bool SetText(const wxString& text) const;

			// Value
			wxString GetValue() const;
			bool SetValue(const wxString& value) const;

			// Attributes
			size_t GetAttributeCount() const;
			wxString GetAttributeNameAt(size_t index) const;
			wxString GetAttributeValueAt(size_t index) const;
			wxString GetAttribute(const wxString& name) const;
			wxString GetAttribute(const char* name) const;

			bool SetAttribute(const wxString& name, const wxString& value);
			bool SetAttribute(const char* name, const wxString& value);
			bool RemoveAttribute(const wxString& name);
			bool RemoveAttribute(const char* name);
			bool ClearAttributes();

			// Style (CSS) attributes
			wxString GetStyleAttribute(const wxString& name) const;
			wxString GetStyleAttribute(const char* name) const;

			bool SetStyleAttribute(const wxString& name, const wxString& value);
			bool SetStyleAttribute(const char* name, const wxString& value);

			bool SetStyleAttribute(const wxString& name, const char* value)
			{
				return SetStyleAttribute(name, wxString(value));
			}
			bool SetStyleAttribute(const char* name, const char* value)
			{
				return SetStyleAttribute(name, wxString(value));
			}

			bool SetStyleAttribute(const wxString& name, const wchar_t* value)
			{
				return SetStyleAttribute(name, wxString(value));
			}
			bool SetStyleAttribute(const char* name, const wchar_t* value)
			{
				return SetStyleAttribute(name, wxString(value));
			}

			bool SetStyleAttribute(const wxString& name, const KxColor& value);
			bool SetStyleAttribute(const char* name, const KxColor& value);

			bool SetStyleAttribute(const wxString& name, int value, SizeUnit unit = SizeUnit::None);
			bool SetStyleAttribute(const char* name, int value, SizeUnit unit = SizeUnit::None);

			bool SetStyleAttribute(const wxString& name, double value, SizeUnit unit = SizeUnit::None);
			bool SetStyleAttribute(const char* name, double value, SizeUnit unit = SizeUnit::None);

			bool RemoveStyleAttribute(const wxString& name);
			bool RemoveStyleAttribute(const char* name);

			bool SetStyleFont(const wxFont& font);

			// Selectors
			size_t Select(const wxString& query, TOnElement onElement) const;
			Element SelectAny(const wxString& query) const;
			std::vector<Element> SelectAll(const wxString& query) const;

			Element GetElementByAttribute(const wxString& name, const wxString& value) const
			{
				return SelectAny(KxString::Format(wxS("[%1=%2]"), name, value));
			}
			Element GetElementByID(const wxString& id) const
			{
				return SelectAny(KxString::Format(wxS("#%1"), id));
			}
			Element GetElementByClass(const wxString& name) const
			{
				return SelectAny(KxString::Format(wxS(".%1"), name));
			}

		public:
			Element& operator=(const Element& other)
			{
				CopyFrom(other);
				return *this;
			}
			Element& operator=(Element&& other)
			{
				MoveFrom(other);
				return *this;
			}
			Element& operator=(ElementHandle* handle)
			{
				CopyFrom(handle);
				return *this;
			}
			
			bool operator==(const Element& other) const
			{
				return m_Handle == other.m_Handle;
			}
			bool operator!=(const Element& other) const
			{
				return !(*this == other);
			}

			explicit operator bool() const
			{
				return IsOk();
			}
			bool operator!() const
			{
				return !IsOk();
			}
	};
}
