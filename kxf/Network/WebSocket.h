#pragma once
#include "Common.h"
#include "WebSocket/Event.h"
#include "WebSocket/IClient.h"
#include "kxf/General/String.h"
#include "kxf/General/Version.h"
#include "URI.h"

namespace kxf::WebSocket
{
	KX_API String GetLibraryName();
	KX_API Version GetLibraryVersion();
}

namespace kxf::WebSocket
{
	KX_API std::unique_ptr<IWebSocketClient> NewSecureClient(const URI& address = {});

	KX_API bool IsAddressWSS(const URI& address);
}
