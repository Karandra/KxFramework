#include "KxfPCH.h"
#include "Common.h"
#include "kxf/General/RegEx.h"
#include "OpenSSL/opensslv.h"

namespace kxf::Crypto
{
	String GetLibraryName()
	{
		return wxS("OpenSSL");
	}
	Version GetLibraryVersion()
	{
		RegEx reg(R"(OpenSSL(?:\s+)([\d+\w+\.]+))");
		if (reg.Matches(OPENSSL_VERSION_TEXT))
		{
			return String(reg.GetMatch(OPENSSL_VERSION_TEXT, 1));
		}
		return OPENSSL_VERSION_TEXT;
	}
}
