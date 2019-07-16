/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxWebSocket.h"
#include "WebSocket/SecureClient.h"
#include "websocketpp/version.hpp"

namespace KxWebSocket
{
	wxString GetLibraryName()
	{
		std::string_view name(websocketpp::user_agent);
		size_t pos = name.rfind('/');
		if (pos != name.npos && pos != 0)
		{
			name.remove_suffix(name.length() - pos);
		}
		return wxString(name.data(), name.size());
	}
	wxString GetLibraryVersion()
	{
		return KxString::Format(wxS("%1.%2.%3"), websocketpp::major_version, websocketpp::minor_version, websocketpp::patch_version);
	}
}

namespace KxWebSocket
{
	std::unique_ptr<KxIWebSocketClient> NewSecureClient(const KxURL& address)
	{
		return std::make_unique<SecureClient>(address);
	}

	bool IsAddressWSS(const KxURL& address)
	{
		return address.HasScheme() && address.GetScheme() == wxS("wss");
	}
}
