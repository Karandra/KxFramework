#pragma once
#include "Common.h"
#include <limits>

namespace kxf
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
		constexpr size_t GetHash() const noexcept
		{
			size_t hash = Data1;
			hash ^= Data2;
			hash ^= Data3;
			for (uint8_t d4: Data4)
			{
				hash ^= d4;
			}

			return hash;
		}

		explicit constexpr operator bool() const noexcept
		{
			return !IsNull();
		}
		constexpr bool operator!() const noexcept
		{
			return IsNull();
		}

		constexpr bool operator==(const NativeUUID& other) const noexcept
		{
			if (this == &other)
			{
				return true;
			}
			else if (Data1 == other.Data1 && Data2 == other.Data2 && Data3 == other.Data3)
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

namespace std
{
	template<>
	struct numeric_limits<kxf::NativeUUID>
	{
		static constexpr bool is_specialized = true;
		static constexpr bool is_exact = true;
		static constexpr bool is_bounded = true;
		static constexpr bool is_integer = true;
		static constexpr bool is_signed = false;
		static constexpr int radix = 2;

		static constexpr kxf::NativeUUID min() noexcept
		{
			return {};
		}
		static constexpr kxf::NativeUUID lowest() noexcept
		{
			return min();
		}
		static constexpr kxf::NativeUUID max() noexcept
		{
			return {0xffffffff, 0xffff, 0xffff, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};
		}
	};
}
