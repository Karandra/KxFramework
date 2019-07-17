#include "KxStdAfx.h"
#include "KxFramework/KxAuiToolBarEvent.h"
#include "KxFramework/KxAuiToolBarItem.h"

KxEVENT_DEFINE_GLOBAL(AUI_TOOLBAR_CLICK, KxAuiToolBarEvent);
KxEVENT_DEFINE_GLOBAL(AUI_TOOLBAR_RIGHT_CLICK, KxAuiToolBarEvent);
KxEVENT_DEFINE_GLOBAL(AUI_TOOLBAR_MIDDLE_CLICK, KxAuiToolBarEvent);
KxEVENT_DEFINE_GLOBAL(AUI_TOOLBAR_DROPDOWN, KxAuiToolBarEvent);
KxEVENT_DEFINE_GLOBAL(AUI_TOOLBAR_OVERFLOW, KxAuiToolBarEvent);

wxIMPLEMENT_DYNAMIC_CLASS(KxAuiToolBarEvent, wxAuiToolBarEvent);

KxAuiToolBarEvent::KxAuiToolBarEvent(const wxAuiToolBarEvent& other)
{
	SetId(other.GetId());
	SetInt(other.GetInt());
	SetExtraLong(other.GetExtraLong());
	SetString(other.GetString());
	SetEventType(other.GetEventType());

	SetDropDownClicked(other.IsDropDownClicked());
	SetClickPoint(other.GetClickPoint());
	SetItemRect(other.GetItemRect());
	SetToolId(other.GetToolId());
}
KxAuiToolBarEvent::KxAuiToolBarEvent(wxEventType type, wxWindowID id)
	:wxAuiToolBarEvent(type, id)
{
}
KxAuiToolBarEvent::~KxAuiToolBarEvent()
{
}

KxAuiToolBarEvent* KxAuiToolBarEvent::Clone() const
{
	return new KxAuiToolBarEvent(*this);
}
