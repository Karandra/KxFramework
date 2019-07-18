#include "KxStdAfx.h"
#include "KxFramework/KxURI.h"
#include "KxFramework/KxString.h"

namespace Kx::Network
{
	wxString NormalizeAddress(const wxString& address)
	{
		if (!address.IsEmpty())
		{
			wxString normalized = KxString::ToLower(address);
			KxString::Trim(normalized, true, true);

			return normalized;
		}
		return address;
	}
}
