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
			name.remove_prefix(pos);
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
	std::unique_ptr<IClient> NewSecureClient(const wxString& address)
	{
		return std::make_unique<SecureClient>(address);
	}

	bool IsAddressWSS(const wxString& address)
	{
		size_t pos = address.find(':');
		if (pos != std::string::npos)
		{
			// For 'wss://example.com:80' pos will be '3'.
			return pos == 3;
		}
		return false;
	}
}
