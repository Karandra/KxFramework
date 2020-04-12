#include "KxStdAfx.h"
#include "Common.h"
#include "OpenSSL/opensslv.h"

namespace KxFramework::Crypto
{
	String GetLibraryName()
	{
		return wxS("OpenSSL");
	}
	Version GetLibraryVersion()
	{
		wxRegEx reg(R"(OpenSSL(?:\s+)([\d+\w+\.]+))", wxRE_ICASE|wxRE_ADVANCED);
		if (reg.Matches(OPENSSL_VERSION_TEXT))
		{
			return String(reg.GetMatch(OPENSSL_VERSION_TEXT, 1));
		}
		return OPENSSL_VERSION_TEXT;
	}
}
