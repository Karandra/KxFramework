#include "KxStdAfx.h"
#include "KxFramework/KxWebSocketEvent.h"

KxWebSocketEvent::KxWebSocketEvent(wxEventType type, wxWindowID winid)
	:wxNotifyEvent(type, winid)
{
	Allow();
}

//////////////////////////////////////////////////////////////////////////
KxEVENT_DEFINE_GLOBAL(KxWebSocketEvent, WEBSOCKET_OPEN);
KxEVENT_DEFINE_GLOBAL(KxWebSocketEvent, WEBSOCKET_FAIL);
KxEVENT_DEFINE_GLOBAL(KxWebSocketEvent, WEBSOCKET_CLOSE);
KxEVENT_DEFINE_GLOBAL(KxWebSocketEvent, WEBSOCKET_MESSAGE);
KxEVENT_DEFINE_GLOBAL(KxWebSocketEvent, WEBSOCKET_CONNECTING);
