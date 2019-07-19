#include "KxStdAfx.h"
#include "KxFramework/KxURI.h"
#include "KxFramework/KxString.h"

namespace Kx::Network
{
	wxString NormalizeInputAddress(const wxString& address)
	{
		if (!address.IsEmpty())
		{
			return KxString::Trim(address, true, true);
		}
		return address;
	}
	void NormalizeURI(wxString& scheme,
					  wxString& server,
					  wxString& path,
					  wxString& query,
					  wxString& fragment,
					  wxString& userInfo,
					  wxString& port
	)
	{
		KxString::MakeLower(scheme);
		KxString::MakeLower(server);
	}
}
