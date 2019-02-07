/*
Copyright © 2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWebSocketEvent.h"
#include "KxFramework/WebSocket/Common.h"
#include "KxFramework/WebSocket/IClient.h"

namespace KxWebSocket
{
	wxString GetLibraryName();
	wxString GetLibraryVersion();
}

namespace KxWebSocket
{
	std::unique_ptr<IClient> NewSecureClient(const wxString& address = {});

	bool IsAddressWSS(const wxString& address);
}
