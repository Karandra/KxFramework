#pragma once
#include "Common.h"
#include "URI.h"
#include "WebSocketEvent.h"
#include "IWebSocketClient.h"
#include "Kx/General/String.h"
#include "Kx/General/Version.h"

namespace KxFramework::WebSocket
{
	String GetLibraryName();
	Version GetLibraryVersion();
}

namespace KxFramework::WebSocket
{
	std::unique_ptr<IWebSocketClient> NewSecureClient(const URI& address = {});

	bool IsAddressWSS(const URI& address);
}
