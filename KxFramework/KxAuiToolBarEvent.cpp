#include "KxStdAfx.h"
#include "KxFramework/KxAuiToolBarEvent.h"
#include "KxFramework/KxAuiToolBarItem.h"

KxEVENT_DEFINE_GLOBAL(KxAuiToolBarEvent, AUI_TOOLBAR_CLICK);
KxEVENT_DEFINE_GLOBAL(KxAuiToolBarEvent, AUI_TOOLBAR_RIGHT_CLICK);
KxEVENT_DEFINE_GLOBAL(KxAuiToolBarEvent, AUI_TOOLBAR_MIDDLE_CLICK);
KxEVENT_DEFINE_GLOBAL(KxAuiToolBarEvent, AUI_TOOLBAR_DROPDOWN);
KxEVENT_DEFINE_GLOBAL(KxAuiToolBarEvent, AUI_TOOLBAR_OVERFLOW);

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
