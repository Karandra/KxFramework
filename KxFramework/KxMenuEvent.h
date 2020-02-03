#pragma once
#include "KxFramework/KxFramework.h"
#include "KxEvent.h"
class KX_API KxMenu;
class KX_API KxMenuItem;

class KX_API KxMenuEvent: public wxNotifyEvent
{
	private:
		bool m_IsPopup = false;
		KxMenu* m_Menu = nullptr;
		KxMenuItem* m_Item = nullptr;
		wxPoint m_Position = wxDefaultPosition;
		wxString m_HelpString;

	public:
		KxMenuEvent(wxEventType type = wxEVT_NULL, KxMenu* menu = nullptr, KxMenuItem* menuItem = nullptr);
		~KxMenuEvent();

	public:
		KxMenuEvent* Clone() const override;

		bool IsPopup() const
		{
			return m_IsPopup;
		}
		void SetPopup(bool isPopup)
		{
			m_IsPopup = isPopup;
		}
		
		KxMenu* GetMenu() const
		{
			return m_Menu;
		}
		void SetMenu(KxMenu* menu)
		{
			m_Menu = menu;
		}
		wxWindow* GetInvokingWindow() const;

		KxMenuItem* GetItem() const
		{
			return m_Item;
		}
		void SetItem(KxMenuItem* menu)
		{
			m_Item = menu;
		}
		
		wxPoint GetPosition() const
		{
			return m_Position;
		}
		void SetPosition(const wxPoint& position)
		{
			m_Position = position;
		}
		
		wxString GetCommandString() const
		{
			return GetString();
		}
		
		const wxString& GetHelpString() const;
		void SetHelpString(const wxString& helpString)
		{
			m_HelpString = helpString;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxMenuEvent);
};

//////////////////////////////////////////////////////////////////////////
KxEVENT_DECLARE_GLOBAL(KxMenuEvent, MENU_OPEN);
KxEVENT_DECLARE_GLOBAL(KxMenuEvent, MENU_CLOSE);
KxEVENT_DECLARE_GLOBAL(KxMenuEvent, MENU_SELECT);
KxEVENT_DECLARE_GLOBAL(KxMenuEvent, MENU_HOVER);
