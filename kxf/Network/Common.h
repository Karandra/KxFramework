#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include "kxf/General/DateTime.h"

namespace kxf
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
	Kx_DeclareFlagSet(URIFlag);

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
}

namespace kxf::Network
{
	bool IsInternetAvailable() noexcept;
	String LookupIP(const URI& uri, NetworkHostType ip);
}
