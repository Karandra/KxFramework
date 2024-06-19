#pragma once
#include "Common.h"
#include "kxf/Serialization/BinarySerializer.h"
#include <cuchar>

namespace kxf
{
	class UniChar final
	{
		private:
			char32_t m_Value = 0;

		public:
			constexpr UniChar(char32_t c = 0) noexcept
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
				return m_Value < 0x80u;
			}
			constexpr bool IsBMP() const noexcept
			{
				return m_Value < 0x10000u;
			}
			constexpr bool IsSupplementary() const noexcept
			{
				return m_Value >= 0x10000u && m_Value <= 0x10FFFFu;
			}
			constexpr bool IsWhitespace() const noexcept
			{
				switch (m_Value)
				{
					case 0x0009: // Character tabulation
					case 0x000A: // Line feed
					case 0x000B: // Line tabulation
					case 0x000C: // Form feed
					case 0x000D: // Carriage return
					case 0x0020: // Space
					case 0x0085: // Next line
					case 0x00A0: // No-break space
					case 0x1680: // Ogham space mark
					case 0x2000: // En quad
					case 0x2001: // Em quad
					case 0x2002: // En space
					case 0x2003: // Em space
					case 0x2004: // Three-per-em space
					case 0x2005: // Four-per-em space
					case 0x2006: // Six-per-em space
					case 0x2007: // Figure space
					case 0x2008: // Punctuation space
					case 0x2009: // Thin space
					case 0x200A: // Hair space
					case 0x2028: // Line separator
					case 0x2029: // Paragraph separator
					case 0x202F: // Narrow no-break space
					case 0x205F: // Medium mathematical space
					case 0x3000: // Ideographic space
					{
						return true;
					}
				};
				return false;
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
					return static_cast<uint16_t>(0xDC00u | ((m_Value - 0x10000u) & 0x03FFu));
				}
				return {};
			}
			constexpr std::optional<uint16_t> ToHighSurrogate() const noexcept
			{
				if (IsSupplementary())
				{
					return static_cast<uint16_t>(0xD800u | ((m_Value - 0x10000u) >> 10));
				}
				return {};
			}

			constexpr char32_t GetValue() const noexcept
			{
				return m_Value;
			}
			constexpr void SetValue(char32_t c) noexcept
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

			constexpr std::strong_ordering Compare(const UniChar& other) const noexcept
			{
				return m_Value <=> other.m_Value;
			}
			std::strong_ordering CompareNoCase(const UniChar& other) const noexcept
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
			constexpr char32_t operator*() const noexcept
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
			return std::hash<uint32_t>()(static_cast<uint32_t>(c.GetValue()));
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
			return Serialization::WriteObject(stream, c.GetAs<uint32_t>());
		}
		uint64_t Deserialize(IInputStream& stream, UniChar& c) const
		{
			uint32_t value = 0;
			auto read = Serialization::ReadObject(stream, value);
			c.SetValue(value);

			return read;
		}
	};
}
