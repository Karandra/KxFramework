#include "KxfPCH.h"
#include "IWebSession.h"

namespace
{
	constexpr kxf::XChar g_UserAgent[] = wxS("User-Agent");
}

namespace kxf
{
	void IWebSession::SetCommonUserAgent(const String& value)
	{
		SetCommonHeader(g_UserAgent, value);
	}
}
