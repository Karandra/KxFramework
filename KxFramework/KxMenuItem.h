#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxMenuEvent.h"
class KxMenu;

class KX_API KxMenuItem: public wxEvtHandler, public wxMenuItem
{
	friend class KxMenu;

	protected:
		template<class T> constexpr static bool TestIDType()
		{
			return std::is_integral_v<T> || std::is_enum_v<T>;
		}

	private:
		wxWindowID m_EffectiveID = KxID_NONE;

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
		KxMenuItem(T id, const wxString& label = {}, const wxString& helpString = {}, wxItemKind kind = wxITEM_NORMAL)
			:KxMenuItem(static_cast<wxWindowID>(id), label, helpString, kind)
		{
		}

		KxMenuItem(wxWindowID id, const wxString& label = {}, const wxString& helpString = {}, wxItemKind kind = wxITEM_NORMAL);
		KxMenuItem(const wxString& label, const wxString& helpString = {}, wxItemKind kind = wxITEM_NORMAL);
		virtual ~KxMenuItem();

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
		wxMenu* GetWxMenu() const
		{
			return wxMenuItem::GetMenu();
		}
		wxMenu* GetWxSubMenu() const
		{
			return wxMenuItem::GetSubMenu();
		}
		KxMenu* GetMenu() const;
		KxMenu* GetSubMenu() const;

		void SetMenu(KxMenu* menu);
		void SetSubMenu(KxMenu* subMenu);

		virtual void SetBitmap(const wxBitmap& normal, const wxBitmap& checked = wxNullBitmap);
		virtual bool SetDefault();

	public:
		wxDECLARE_DYNAMIC_CLASS(KxMenuItem);
};

template<class ItemT, class MenuT>
class KxMenuItemIterator
{
	static_assert(std::is_base_of<wxMenu, MenuT>::value, "MenuT must be derived from wxMenu");

	public:
		using self_type = KxMenuItemIterator<ItemT, MenuT>;
		using value_type = ItemT;
		using reference = ItemT&;
		using pointer = ItemT*;
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using size_type = std::size_t;

	private:
		MenuT* m_Menu = nullptr;
		size_type m_ItemIndex = 0;

	private:
		ItemT* GetItemByIndex(size_type index) const
		{
			return static_cast<ItemT*>(m_Menu->GetMenuItems()[index]);
		}

	public:
		KxMenuItemIterator(MenuT* menu, size_type index = 0)
			:m_Menu(menu), m_ItemIndex(index)
		{
		}

	public:
		self_type operator++()
		{
			auto& items = m_Menu->GetMenuItems();
			if (m_ItemIndex + 1 < items.size())
			{
				return self_type(m_Menu, m_ItemIndex + 1);
			}
			return self_type(m_Menu, items.size());
		}
		self_type operator++(int)
		{
			auto& items = m_Menu->GetMenuItems();
			if (m_ItemIndex + 1 < items.size())
			{
				++m_ItemIndex;
			}
			return *this;
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
		
		bool operator==(const self_type& rhs) const
		{
			return m_Menu == rhs.m_Menu && m_ItemIndex == rhs.m_ItemIndex;
		}
		bool operator!=(const self_type& rhs) const
		{
			return !(*this == rhs);
		}
};
