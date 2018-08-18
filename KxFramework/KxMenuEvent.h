#pragma once
#include "KxFramework/KxFramework.h"
class KxMenu;
class KxMenuItem;

class KxMenuEvent: public wxNotifyEvent
{
	private:
		bool m_IsPopup = false;
		KxMenu* m_Menu = NULL;
		KxMenuItem* m_Item = NULL;
		wxPoint m_Position = wxDefaultPosition;
		wxString m_HelpString;

	public:
		KxMenuEvent(wxEventType type = wxEVT_NULL, KxMenu* menu = NULL, KxMenuItem* menuItem = NULL);
		virtual ~KxMenuEvent();
		virtual KxMenuEvent* Clone() const;

	public:
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
wxDECLARE_EVENT(KxEVT_MENU_OPEN, KxMenuEvent);
wxDECLARE_EVENT(KxEVT_MENU_CLOSE, KxMenuEvent);
wxDECLARE_EVENT(KxEVT_MENU_SELECT, KxMenuEvent);
wxDECLARE_EVENT(KxEVT_MENU_HOVER, KxMenuEvent);
