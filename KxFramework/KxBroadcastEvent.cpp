#include "KxStdAfx.h"
#include "KxFramework/KxBroadcastEvent.h"

KxBroadcastEvent::KxBroadcastEvent(wxEventType type)
	:wxNotifyEvent(type)
{
	// Default behavior is to walk this event through entire handlers tree to the wxApp
	wxNotifyEvent::StopPropagation();
	wxNotifyEvent::Skip();
}
KxBroadcastEvent::~KxBroadcastEvent()
{
}
KxBroadcastEvent* KxBroadcastEvent::Clone() const
{
	return new KxBroadcastEvent(*this);
}
