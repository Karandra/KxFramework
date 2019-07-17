#include "KxStdAfx.h"
#include "KxFramework/KxMenuEvent.h"
#include "KxFramework/KxMenu.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxMenuEvent, wxNotifyEvent);

KxEVENT_DEFINE_GLOBAL(MENU_OPEN, KxMenuEvent);
KxEVENT_DEFINE_GLOBAL(MENU_CLOSE, KxMenuEvent);
KxEVENT_DEFINE_GLOBAL(MENU_SELECT, KxMenuEvent);
KxEVENT_DEFINE_GLOBAL(MENU_HOVER, KxMenuEvent);

KxMenuEvent::KxMenuEvent(wxEventType type, KxMenu* menu, KxMenuItem* menuItem)
	:wxNotifyEvent(type, menuItem ? menuItem->GetId() : wxID_NONE), m_Menu(menu), m_Item(menuItem)
{
	Allow();
}
KxMenuEvent::~KxMenuEvent()
{
}
KxMenuEvent* KxMenuEvent::Clone() const
{
	return new KxMenuEvent(*this);
}

const wxString& KxMenuEvent::GetHelpString() const
{
	return m_Item ? m_Item->GetHelp() : m_HelpString;
}
