#pragma once
#include "Common.h"
#include "WebSocket/Event.h"
#include "WebSocket/IClient.h"
#include "Kx/General/String.h"
#include "Kx/General/Version.h"
#include "URI.h"

namespace kxf::WebSocket
{
	String GetLibraryName();
	Version GetLibraryVersion();
}

namespace kxf::WebSocket
{
	std::unique_ptr<IWebSocketClient> NewSecureClient(const URI& address = {});

	bool IsAddressWSS(const URI& address);
}
