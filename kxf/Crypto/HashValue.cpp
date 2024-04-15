#include "KxfPCH.h"
#include "HashValue.h"
#include "kxf/Core/String.h"
#include "kxf/Core/Format.h"

namespace kxf::Crypto::Private
{
	String HashValueToString(std::span<const std::byte> data)
	{
		String result;
		result.reserve(data.size() * 2);

		for (std::byte x: data)
		{
			result += Format("{:02x}", x);
		};
		return result;
	}
}
