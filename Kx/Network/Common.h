#pragma once
#include "Kx/Common.hpp"
#include "Kx/General/String.h"
#include "Kx/General/DateTime.h"

namespace KxFramework
{
	class URI;
	class URL;

	enum class NetworkHostType
	{
		None = -1,

		RegName,
		IPvUnknown,
		IPv4,
		IPv6,
	};
	enum class URIFlag
	{
		None = 0,
		Strict = 1 << 0
	};
	enum class URLStatus
	{
		Success = 0,
		Unknown,
		Syntax,
		NoProtocol,
		NoHost,
		NoPath,
		ConnectionError,
		ProtocolError
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(URIFlag);
	}
}

namespace KxFramework::Network
{
	bool IsInternetAvailable() noexcept;
	String LookupIP(const URI& uri, NetworkHostType ip);
}
