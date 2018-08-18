#include "KxStdAfx.h"
#include "KxFramework/KxWebSocketEvent.h"

KxWebSocketEvent::KxWebSocketEvent(wxEventType type, wxWindowID winid)
	:wxNotifyEvent(type, winid)
{
	Allow();
}

//////////////////////////////////////////////////////////////////////////
wxDEFINE_EVENT(KxEVT_WEBSOCKET_OPEN, KxWebSocketEvent);
wxDEFINE_EVENT(KxEVT_WEBSOCKET_FAIL, KxWebSocketEvent);
wxDEFINE_EVENT(KxEVT_WEBSOCKET_CLOSE, KxWebSocketEvent);
wxDEFINE_EVENT(KxEVT_WEBSOCKET_MESSAGE, KxWebSocketEvent);
