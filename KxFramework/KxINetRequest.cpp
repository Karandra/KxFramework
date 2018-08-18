#include "KxStdAfx.h"
#include "KxFramework/KxINetRequest.h"
#include "KxFramework/KxINet.h"
#include "KxFramework/KxINetConnection.h"
#include "KxFramework/KxINetEvent.h"
#include <WinINet.h>

wxString KxINetRequest::ConstructFullURL(const wxString& baseURL) const
{
	if (!m_URLItems.empty())
	{
		wxString out;
		for (const auto& v: m_URLItems)
		{
			out += wxString::Format("%s%c%s%c", v.first, m_ValueDelimiter, v.second, m_PairDelimiter);
		}
		if (out.Last() == m_PairDelimiter)
		{
			out.RemoveLast(1);
		}
		return baseURL + out;
	}
	return baseURL;
}
