/*
Copyright � 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "IClient.h"
#include "SecureClient.h"
#include "websocketpp/version.hpp"

namespace KxWebSocket
{
	wxString IClient::GetLibraryName()
	{
		std::string_view name(websocketpp::user_agent);
		size_t pos = name.rfind('/');
		if (pos != name.npos && pos != 0)
		{
			name.remove_prefix(pos);
		}
		return wxString(name.data(), name.size());
	}
	wxString IClient::GetLibraryVersion()
	{
		return KxString::Format(wxS("%1.%2.%3"), websocketpp::major_version, websocketpp::minor_version, websocketpp::patch_version);
	}

	std::unique_ptr<IClient> IClient::NewSecureClient(const wxString& address)
	{
		return std::make_unique<SecureClient>(address);
	}

	bool IClient::IsAddressWSS(const wxString& address) const
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
