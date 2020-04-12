#pragma once
#include "Kx/Common.hpp"
#include "Kx/General/String.h"
class KxURI;

namespace KxFramework::Network
{
	enum class IP
	{
		v4,
		v6
	};
}

namespace KxFramework::Network
{
	bool IsInternetAvailable() noexcept;
	String LookupIP(const KxURI& uri, IP ip = IP::v4);
}
