#pragma once
#include "Common.h"
#include "Kx/General/String.h"
#include <array>

namespace KxFramework::Crypto::Private
{
	template<size_t bitLength, class T>
	constexpr bool IsHashConvertibleToInteger() noexcept
	{
		return std::is_integral_v<T> && std::is_unsigned_v<T> && (sizeof(T) <= bitLength / 8) && (bitLength == 8 || bitLength == 16 || bitLength == 32 || bitLength == 64);
	}
}

namespace KxFramework::Crypto
{
	template<size_t bitLength>
	class HashValue final
	{
		public:
			static constexpr size_t bit_length() noexcept
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
				wxString result;
				result.reserve(m_Hash.size() * 2);

				for (uint8_t x : m_Hash)
				{
					result.sprintf(wxS("%s%02x"), result, x);
				};
				return result;
			}

			template<class = std::enable_if_t<Private::IsHashConvertibleToInteger<bitLength, uint8_t>()>>
			auto ToInt() const noexcept
			{
				if constexpr(bitLength == 8)
				{
					return *reinterpret_cast<const uint8_t*>(m_Hash.data());
				}
				else if constexpr(bitLength == 16)
				{
					return *reinterpret_cast<const uint16_t*>(m_Hash.data());
				}
				else if constexpr(bitLength == 32)
				{
					return *reinterpret_cast<const uint32_t*>(m_Hash.data());
				}
				else if constexpr(bitLength == 64)
				{
					return *reinterpret_cast<const uint64_t*>(m_Hash.data());
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
			constexpr bool operator!=(const HashValue& other) const noexcept
			{
				return m_Hash != other.m_Hash;
			}
	};
}
