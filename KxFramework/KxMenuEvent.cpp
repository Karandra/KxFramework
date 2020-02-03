#include "KxStdAfx.h"
#include "KxFramework/KxMenuEvent.h"
#include "KxFramework/KxMenu.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxMenuEvent, wxNotifyEvent);

KxEVENT_DEFINE_GLOBAL(KxMenuEvent, MENU_OPEN);
KxEVENT_DEFINE_GLOBAL(KxMenuEvent, MENU_CLOSE);
KxEVENT_DEFINE_GLOBAL(KxMenuEvent, MENU_SELECT);
KxEVENT_DEFINE_GLOBAL(KxMenuEvent, MENU_HOVER);

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
wxWindow* KxMenuEvent::GetInvokingWindow() const
{
	return m_Menu ? m_Menu->GetWindow() : nullptr;
}
const wxString& KxMenuEvent::GetHelpString() const
{
	return m_Item ? m_Item->GetHelp() : m_HelpString;
}
