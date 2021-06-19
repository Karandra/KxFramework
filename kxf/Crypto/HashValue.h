#pragma once
#include "Common.h"
#include <array>

namespace kxf::Crypto::Private
{
	template<size_t bitLength, class T>
	constexpr bool IsHashConvertibleToInteger() noexcept
	{
		return std::is_integral_v<T> && std::is_unsigned_v<T> && (sizeof(T) <= bitLength / 8) && (bitLength == 8 || bitLength == 16 || bitLength == 32 || bitLength == 64);
	}

	String HashValueToString(std::span<const uint8_t> data);
}

namespace kxf::Crypto
{
	template<size_t bitLength>
	class HashValue final
	{
		static_assert(bitLength >= 8 && (static_cast<size_t>(bitLength / 8.0) * 8 == bitLength), "hash value length should be >= 8 bits and evenly divisible by 8");

		public:
			static constexpr size_t BitLength() noexcept
			{
				return bitLength;
			}

		private:
			std::array<uint8_t, bitLength / 8> m_Hash;

		public:
			constexpr HashValue() noexcept = default;
			HashValue(const void* data, size_t length) noexcept
			{
				if (length == m_Hash.size())
				{
					std::memcpy(m_Hash.data(), data, length);
				}
			}

			template<class T, class = std::enable_if_t<Private::IsHashConvertibleToInteger<bitLength, T>()>>
			HashValue(T intValue)
				:HashValue(&intValue, sizeof(intValue))
			{
			}

		public:
			constexpr size_t length() const noexcept
			{
				return m_Hash.size();
			}
			constexpr const uint8_t* data() const noexcept
			{
				return m_Hash.data();
			}
			constexpr uint8_t* data() noexcept
			{
				return m_Hash.data();
			}

			String ToString() const
			{
				return Private::HashValueToString({m_Hash.data(), m_Hash.size()});
			}

			template<class = std::enable_if_t<Private::IsHashConvertibleToInteger<bitLength, uint8_t>()>>
			auto ToInt() const noexcept
			{
				auto Convert = [](auto& value)
				{
					std::memcpy(&value, m_Hash.data(), sizeof(value));
					return value;
				};

				if constexpr(bitLength == 8)
				{
					uint8_t value = 0;
					return Convert(value);
				}
				else if constexpr(bitLength == 16)
				{
					uint16_t value = 0;
					return Convert(value);
				}
				else if constexpr(bitLength == 32)
				{
					uint32_t value = 0;
					return Convert(value);
				}
				else if constexpr(bitLength == 64)
				{
					uint64_t value = 0;
					return Convert(value);
				}
				else
				{
					static_assert(false, "unsupported bit length");
				}
			}

		public:
			constexpr bool operator==(const HashValue& other) const noexcept
			{
				return m_Hash == other.m_Hash;
			}
	};
}
