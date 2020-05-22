#pragma once
#include "Kx/UI/Common.h"
#include "Kx/EventSystem/Event.h"

namespace KxFramework::UI
{
	class Menu;
	class MenuItem;
}

namespace KxFramework::UI
{
	class KX_API MenuEvent: public wxNotifyEvent
	{
		public:
			KxEVENT_MEMBER(MenuEvent, Open);
			KxEVENT_MEMBER(MenuEvent, Close);
			KxEVENT_MEMBER(MenuEvent, Select);
			KxEVENT_MEMBER(MenuEvent, Hover);

		private:
			bool m_IsPopup = false;
			Menu* m_Menu = nullptr;
			MenuItem* m_Item = nullptr;
			Point m_Position = wxDefaultPosition;
			String m_HelpString;

		public:
			MenuEvent(EventID type = Event::EvtNull, Menu* menu = nullptr, MenuItem* menuItem = nullptr);

		public:
			MenuEvent* Clone() const override
			{
				return new MenuEvent(*this);
			}

			bool IsPopup() const
			{
				return m_IsPopup;
			}
			void SetPopup(bool isPopup)
			{
				m_IsPopup = isPopup;
			}
		
			Menu* GetMenu() const
			{
				return m_Menu;
			}
			void SetMenu(Menu* menu)
			{
				m_Menu = menu;
			}
			wxWindow* GetInvokingWindow() const;

			MenuItem* GetItem() const
			{
				return m_Item;
			}
			void SetItem(MenuItem* menu)
			{
				m_Item = menu;
			}
		
			Point GetPosition() const
			{
				return m_Position;
			}
			void SetPosition(const Point& position)
			{
				m_Position = position;
			}
		
			String GetCommandString() const
			{
				return GetString();
			}
			String GetHelpString() const;
			void SetHelpString(const String& helpString)
			{
				m_HelpString = helpString;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(MenuEvent);
	};
}


