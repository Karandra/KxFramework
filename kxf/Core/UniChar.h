#pragma once
#include "Common.h"
#include "kxf/Serialization/BinarySerializer.h"

namespace kxf
{
	class UniChar final
	{
		friend struct std::hash<UniChar>;
		friend struct BinarySerializer<UniChar>;

		private:
			uint32_t m_Value = 0;

		public:
			constexpr UniChar(uint32_t c = 0) noexcept
				:m_Value(c)
			{
			}

		public:
			constexpr bool IsNull() const noexcept
			{
				return m_Value == 0;
			}
			constexpr bool IsASCII() const noexcept
			{
				return m_Value < 0x80;
			}
			constexpr bool IsBMP() const noexcept
			{
				return m_Value < 0x10000;
			}
			constexpr bool IsSupplementary() const noexcept
			{
				return m_Value >= 0x10000 && m_Value <= 0x10FFFF;
			}

			constexpr std::optional<char> ToASCII() const noexcept
			{
				if (IsASCII())
				{
					return static_cast<char>(m_Value);
				}
				return {};
			}
			constexpr std::optional<uint16_t> ToLowSurrogate() const noexcept
			{
				if (IsSupplementary())
				{
					return static_cast<uint16_t>(0xDC00 | ((m_Value - 0x10000) & 0x03FF));
				}
				return {};
			}
			constexpr std::optional<uint16_t> ToHighSurrogate() const noexcept
			{
				if (IsSupplementary())
				{
					return static_cast<uint16_t>(0xD800 | ((m_Value - 0x10000) >> 10));
				}
				return {};
			}

			constexpr uint32_t GetValue() const noexcept
			{
				return m_Value;
			}
			constexpr void SetValue(uint32_t c) noexcept
			{
				m_Value = c;
			}

			template<class T> requires(std::is_integral_v<T> || std::is_enum_v<T>)
			constexpr T GetAs() const noexcept
			{
				return static_cast<T>(m_Value);
			}

			UniChar ToLowerCase() const noexcept;
			UniChar ToUpperCase() const noexcept;

			constexpr std::strong_ordering Compare(const UniChar& other) noexcept
			{
				return m_Value <=> other.m_Value;
			}
			std::strong_ordering CompareNoCase(const UniChar& other) noexcept
			{
				return ToLowerCase().m_Value <=> other.ToLowerCase().m_Value;
			}

		public:
			constexpr explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			constexpr bool operator!() const noexcept
			{
				return IsNull();
			}

			constexpr auto operator<=>(const UniChar&) const noexcept = default;
			constexpr uint32_t operator*() const noexcept
			{
				return m_Value;
			}
	};
}

namespace std
{
	template<>
	struct hash<kxf::UniChar> final
	{
		size_t operator()(const kxf::UniChar& c) const noexcept
		{
			return std::hash<uint32_t>()(c.m_Value);
		}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<UniChar> final
	{
		uint64_t Serialize(IOutputStream& stream, const UniChar& c) const
		{
			return Serialization::WriteObject(stream, c.m_Value);
		}
		uint64_t Deserialize(IInputStream& stream, UniChar& c) const
		{
			return Serialization::ReadObject(stream, c.m_Value);
		}
	};
}
