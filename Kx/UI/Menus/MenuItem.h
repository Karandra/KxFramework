#pragma once
#include "Kx/UI/Common.h"
#include "MenuEvent.h"
#include <wx/menuitem.h>

namespace kxf::UI
{
	class Menu;
}

namespace kxf::UI
{
	class KX_API MenuItem: public wxEvtHandler, public wxMenuItem
	{
		friend class Menu;

		protected:
			template<class T>
			constexpr static bool TestIDType() noexcept
			{
				return std::is_integral_v<T> || std::is_enum_v<T>;
			}

		private:
			wxWindowID m_EffectiveID = wxID_NONE;

		private:
			void OnCreate();
			bool OnMeasureItem(size_t* width, size_t* height) override;
			bool OnDrawItem(wxDC& dc, const wxRect& rect, wxODAction action, wxODStatus status) override;

			void OnAddedToMenu();
			void OnRemovedFromMenu();

			void CheckIfShouldOwnerDraw();
			wxWindowID GetEffectiveID(wxWindowID id) const;

		public:
			template<class T, class = std::enable_if_t<TestIDType<T>()>>
			MenuItem(T id, const String& label = {}, const String& helpString = {}, wxItemKind kind = wxITEM_NORMAL)
				:MenuItem(static_cast<wxWindowID>(id), label, helpString, kind)
			{
			}

			MenuItem(wxWindowID id, const String& label = {}, const String& helpString = {}, wxItemKind kind = wxITEM_NORMAL);
			MenuItem(const String& label, const String& helpString = {}, wxItemKind kind = wxITEM_NORMAL);

		public:
			wxEvtHandler* GetEventHandler()
			{
				return this;
			}
			const wxEvtHandler* GetEventHandler() const
			{
				return this;
			}
		
			bool HasEffectiveID() const
			{
				return m_EffectiveID >= 0;
			}
			wxWindowID GetId() const
			{
				return HasEffectiveID() ? m_EffectiveID : wxMenuItem::GetId();
			}
		
			wxWindow* GetWindow() const;
			wxMenu* GetMenuWx() const
			{
				return wxMenuItem::GetMenu();
			}
			wxMenu* GetSubMenuWx() const
			{
				return wxMenuItem::GetSubMenu();
			}
			Menu* GetMenu() const;
			Menu* GetSubMenu() const;

			void SetMenu(Menu* menu);
			void SetSubMenu(Menu* subMenu);

			virtual void SetBitmap(const wxBitmap& normal, const wxBitmap& checked = wxNullBitmap);
			virtual bool SetDefault();

		public:
			wxDECLARE_DYNAMIC_CLASS(MenuItem);
	};
}

namespace kxf::UI
{
	template<class TItem, class TMenu>
	class MenuItemIterator
	{
		static_assert(std::is_base_of<wxMenu, TMenu>::value, "'TMenu' must be derived from 'wxMenu'");

		public:
			using value_type = TItem;
			using reference = TItem&;
			using pointer = TItem*;
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using size_type = std::size_t;

		private:
			TMenu* m_Menu = nullptr;
			size_type m_ItemIndex = 0;

		private:
			TItem* GetItemByIndex(size_type index) const
			{
				return static_cast<TItem*>(m_Menu->GetMenuItems()[index]);
			}

		public:
			MenuItemIterator(TMenu* menu, size_type index = 0)
				:m_Menu(menu), m_ItemIndex(index)
			{
			}

		public:
			MenuItemIterator operator++()
			{
				auto& items = m_Menu->GetMenuItems();
				if (m_ItemIndex + 1 < items.size())
				{
					++m_ItemIndex;
				}
				return *this;
			}
			MenuItemIterator operator++(int)
			{
				auto& items = m_Menu->GetMenuItems();
				if (m_ItemIndex + 1 < items.size())
				{
					return MenuItemIterator(m_Menu, m_ItemIndex + 1);
				}
				return MenuItemIterator(m_Menu, items.size());
			}

			reference operator*()
			{
				return *GetItemByIndex(m_ItemIndex);
			}
			pointer operator->() const
			{
				auto& items = m_Menu->GetMenuItems();
				return m_ItemIndex < items.size() ? GetItemByIndex(m_ItemIndex) : nullptr;
			}
		
			bool operator==(const MenuItemIterator& rhs) const
			{
				return m_Menu == rhs.m_Menu && m_ItemIndex == rhs.m_ItemIndex;
			}
			bool operator!=(const MenuItemIterator& rhs) const
			{
				return !(*this == rhs);
			}
	};
}
