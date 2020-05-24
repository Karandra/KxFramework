#pragma once
#include "Kx/UI/Common.h"
#include "MenuEvent.h"
#include "MenuItem.h"
#include <wx/menu.h>

namespace kxf::UI
{
	class KX_API Menu: public wxMenu
	{
		public:
			static constexpr FlagSet<Alignment> DefaultAlignment = Alignment::Left|Alignment::Top;

		public:
			static bool EndMenu();
			static Menu* GetCurrentMenu();

		public:
			using iterator = MenuItemIterator<MenuItem, Menu>;
			using const_iterator = MenuItemIterator<const MenuItem, const Menu>;

		private:
			void OnSelectItem(wxCommandEvent& event);
			void OnHoverItem(wxMenuEvent& event);

		protected:
			virtual uint16_t DoShowMenu(wxWindow* window, const Point& pos, FlagSet<Alignment> alignment, bool async);
			void ProcessItemSelection(int menuWxID, wxCommandEvent* event = nullptr);

		public:
			static int WinIDToWx(uint32_t menuWinID)
			{
				return static_cast<int16_t>(menuWinID);
			}
			static uint32_t WxIDToWin(int menuWxID)
			{
				return static_cast<uint32_t>(menuWxID);
			}
			static wxWindowID WinMenuRetToWx(uint16_t menuWinID)
			{
				if (menuWinID != 0)
				{
					return WinIDToWx(menuWinID);
				}
				return wxID_NONE;
			}

		public:
			Menu(const String& label = {}, long style = 0);

		public:
			virtual bool IsOK() const
			{
				return true;
			}

		public:
			wxWindowID Show(wxWindow* window = nullptr, const Point& pos = Point::UnspecifiedPosition(), FlagSet<Alignment> alignment = DefaultAlignment);
			wxWindowID ShowAsPopup(wxWindow* window, int offset = 1, FlagSet<Alignment> alignment = DefaultAlignment);

		public:
			virtual MenuItem* Add(MenuItem* item);
			MenuItem* Add(Menu* subMenu, const String& label, const String& helpString = {});
			MenuItem* AddSeparator();

			virtual MenuItem* Insert(size_t pos, MenuItem* item);
			MenuItem* Insert(size_t pos, Menu* subMenu, const String& label, const String& helpString = {});
			MenuItem* InsertSeparator(size_t pos);

			virtual MenuItem* Prepend(MenuItem* item);
			MenuItem* Prepend(Menu* subMenu, const String& label, const String& helpString = {});
			MenuItem* PrependSeparator();

			MenuItem* FindChildItem(wxWindowID id, size_t* posPtr = nullptr) const;
			MenuItem* FindItem(wxWindowID id, Menu** menu = nullptr) const;
			MenuItem* FindItemByPosition(size_t pos) const;

			virtual MenuItem* RemoveItem(MenuItem* item);
			virtual MenuItem* RemoveItem(wxWindowID id);

		public:
			template<class TItem = MenuItem, class... Args>
			TItem* AddItem(Args&&... arg)
			{
				TItem* item = new TItem(std::forward<Args>(arg)...);
				Add(item);
				return item;
			}
			
			template<class TItem = MenuItem, class... Args>
			TItem* InsertItem(size_t pos, Args&& ... arg)
			{
				TItem* item = new TItem(std::forward<Args>(arg)...);
				Insert(pos, item);
				return item;
			}
			
			template<class TItem = MenuItem, class... Args>
			TItem* PrependItem(Args&& ... arg)
			{
				TItem* item = new TItem(std::forward<Args>(arg)...);
				Prepend(item);
				return item;
			}

		public:
			iterator begin()
			{
				return iterator(this);
			}
			iterator end()
			{
				return iterator(this, GetMenuItemCount());
			}

			const_iterator begin() const
			{
				return const_iterator(this);
			}
			const_iterator end() const
			{
				return const_iterator(this, GetMenuItemCount());
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(Menu);
	};
}
