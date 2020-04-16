#include "stdafx.h"
#include "WebSocket.h"
#include "WebSocket/SecureClient.h"
#include <websocketpp/version.hpp>

namespace KxFramework::WebSocket
{
	String GetLibraryName()
	{
		String userAgent = String::FromUTF8(websocketpp::user_agent);
		return userAgent.BeforeFirst(wxS('/'));
	}
	Version GetLibraryVersion()
	{
		return {websocketpp::major_version, websocketpp::minor_version, websocketpp::patch_version};
	}
}

namespace KxFramework::WebSocket
{
	std::unique_ptr<IWebSocketClient> NewSecureClient(const URI& address)
	{
		auto client = std::make_unique<SecureClient>(address);
		if (!client->IsNull())
		{
			return client;
		}
		return nullptr;
	}

	bool IsAddressWSS(const URI& address)
	{
		return address.HasScheme() && address.GetScheme() == wxS("wss");
	}
}
