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
			static Element Create(const wxString& tagName, const wxString& value = {});

		private:
			static BOOL EventHandler(void* context, ElementHandle* element, uint32_t eventGroupID, void* parameters);

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

			bool Attach(ElementHandle* handle);
			ElementHandle* Detach();
			bool Remove();

			Node ToNode() const;
			Element Clone() const;

			// Event handling
			void AttachEventHandler();
			void DetachEventHandler();

			// Refreshing
			bool Update(bool force = false);
			bool UpdateRect(const wxRect& rect);

			// Size and position
			wxRect GetRect() const;
			wxPoint GetPosition() const;
			wxSize GetSize() const;

			wxSize GetMinSize() const;
			wxSize GetMaxSize() const;

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

			wxString GetInnerHTML() const;
			wxString GetOuterHTML() const;
			bool SetInnerHTML(const wxString& html, ElementInnerHTML mode);
			bool SetOuterHTML(const wxString& html, ElementOuterHTML mode);

			// Children and parents
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

			// Value
			wxString GetValue() const;
			bool SetValue(wxStringView value) const;

			// Attributes
			size_t GetAttributeCount() const;
			wxString GetAttributeNameAt(size_t index) const;
			wxString GetAttributeValueAt(size_t index) const;
			wxString GetAttribute(const wxString& name) const;
			bool SetAttribute(const wxString& name, const wxString& value);
			bool RemoveAttribute(const wxString& name);
			bool ClearAttributes();

			// Style (CSS) attributes
			wxString GetStyleAttribute(const wxString& name) const;
			KxColor GetStyleAttributeColor(const wxString& name) const;
			std::optional<int> GetStyleAttributeInt(const wxString& name) const;
			std::optional<double> GetStyleAttributeFloat(const wxString& name) const;

			bool SetStyleAttribute(const wxString& name, const wxString& value);
			bool SetStyleAttribute(const wxString& name, const char* value)
			{
				return SetStyleAttribute(name, wxString(value));
			}
			bool SetStyleAttribute(const wxString& name, const wchar_t* value)
			{
				return SetStyleAttribute(name, wxString(value));
			}
			bool SetStyleAttribute(const wxString& name, const KxColor& value);
			bool SetStyleAttribute(const wxString& name, int value, SizeUnit unit = SizeUnit::None);
			bool SetStyleAttribute(const wxString& name, double value, SizeUnit unit = SizeUnit::None);
			bool RemoveStyleAttribute(const wxString& name);

			bool SetStyleFont(const wxFont& font);

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
