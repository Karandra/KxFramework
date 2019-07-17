#include "KxStdAfx.h"
#include "KxFramework/KxWebSocketEvent.h"

KxWebSocketEvent::KxWebSocketEvent(wxEventType type, wxWindowID winid)
	:wxNotifyEvent(type, winid)
{
	Allow();
}

//////////////////////////////////////////////////////////////////////////
KxEVENT_DEFINE_GLOBAL(WEBSOCKET_OPEN, KxWebSocketEvent);
KxEVENT_DEFINE_GLOBAL(WEBSOCKET_FAIL, KxWebSocketEvent);
KxEVENT_DEFINE_GLOBAL(WEBSOCKET_CLOSE, KxWebSocketEvent);
KxEVENT_DEFINE_GLOBAL(WEBSOCKET_MESSAGE, KxWebSocketEvent);
KxEVENT_DEFINE_GLOBAL(WEBSOCKET_CONNECTING, KxWebSocketEvent);
