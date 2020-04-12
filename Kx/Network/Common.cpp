#include "KxStdAfx.h"
#include "Common.h"
#include "Kx/Utility/CallAtScopeExit.h"
#include "KxFramework/KxURI.h"

#include <WinINet.h>
#include <WinDNS.h>
#include <ws2tcpip.h>
#pragma comment(lib, "DNSAPI.lib")
#pragma comment(lib, "WinINet.lib")
#include "Kx/System/UndefWindows.h"

namespace KxFramework::Network
{
	bool IsInternetAvailable() noexcept
	{
		DWORD flags = 0;
		return ::InternetGetConnectedState(&flags, 0);
	}
	String LookupIP(const KxURI& uri, IP ip)
	{
		DNS_RECORD* infoDNS = nullptr;
		Utility::CallAtScopeExit atExit([&]()
		{
			if (infoDNS)
			{
				DnsRecordListFree(infoDNS, DnsFreeRecordListDeep);
			}
		});

		String hostName = uri.GetServer();
		if (::DnsQuery_W(hostName.wc_str(), ip == IP::v6 ? DNS_TYPE_A6 : DNS_TYPE_A, DNS_QUERY_STANDARD|DNS_QUERY_BYPASS_CACHE, nullptr, &infoDNS, nullptr) == 0)
		{
			bool isSuccess = false;
			wchar_t buffer[64] = {0};

			if (ip == IP::v6)
			{
				isSuccess = ::InetNtopW(AF_INET6, &infoDNS->Data.AAAA.Ip6Address, buffer, std::size(buffer)) != nullptr;
			}
			else
			{
				isSuccess = ::InetNtopW(AF_INET, &infoDNS->Data.A.IpAddress, buffer, std::size(buffer)) != nullptr;
			}

			if (isSuccess)
			{
				return buffer;
			}
		}
		return {};
	}
}
