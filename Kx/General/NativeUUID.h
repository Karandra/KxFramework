#pragma once
#include "Common.h"

namespace KxFramework
{
	struct NativeUUID final
	{
		uint32_t Data1 = 0;
		uint16_t Data2 = 0;
		uint16_t Data3 = 0;
		uint8_t Data4[8] = {};

		constexpr bool IsNull() const
		{
			return *this == NativeUUID{0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};
		}
		constexpr NativeUUID& MakeNull()
		{
			Data1 = 0;
			Data2 = 0;
			Data3 = 0;
			for (uint8_t& d4i: Data4)
			{
				d4i = 0;
			}

			return *this;
		}

		constexpr bool operator==(const NativeUUID& other) const noexcept
		{
			if (Data1 == other.Data1 && Data2 == other.Data2 && Data3 == other.Data3)
			{
				for (size_t i = 0; i < sizeof(NativeUUID::Data4); i++)
				{
					if (Data4[i] != other.Data4[i])
					{
						return false;
					}
				}
				return true;
			}
			return false;
		}
		constexpr bool operator!=(const NativeUUID& other) const noexcept
		{
			return !(*this == other);
		}
	};
}
