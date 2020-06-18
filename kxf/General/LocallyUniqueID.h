#pragma once
#include "Common.h"
#include "String.h"

namespace kxf
{
	class KX_API LocallyUniqueID final
	{
		public:
			static LocallyUniqueID CreateSequential() noexcept;

		private:
			uint64_t m_ID = 0;

		public:
			constexpr LocallyUniqueID() noexcept = default;
			constexpr LocallyUniqueID(uint64_t value) noexcept
				:m_ID(value)
			{
			}
			LocallyUniqueID(const char* value) noexcept;
			LocallyUniqueID(const wchar_t* value) noexcept;
			LocallyUniqueID(const String& value) noexcept;

		public:
			constexpr bool IsNull() const noexcept
			{
				return m_ID == 0;
			}

			constexpr uint64_t ToInt() const noexcept
			{
				return m_ID;
			}
			String ToString() const;

		public:
			operator String() const
			{
				return ToString();
			}

			constexpr explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			constexpr bool operator!() const noexcept
			{
				return IsNull();
			}

		public:
			constexpr bool operator==(const LocallyUniqueID& other) const noexcept
			{
				return m_ID == other.m_ID;
			}
			constexpr bool operator!=(const LocallyUniqueID& other) const noexcept
			{
				return m_ID != other.m_ID;
			}
			constexpr bool operator<(const LocallyUniqueID& other) const noexcept
			{
				return m_ID < other.m_ID;
			}
			constexpr bool operator<=(const LocallyUniqueID& other) const noexcept
			{
				return m_ID <= other.m_ID;
			}
			constexpr bool operator>(const LocallyUniqueID& other) const noexcept
			{
				return m_ID > other.m_ID;
			}
			constexpr bool operator>=(const LocallyUniqueID& other) const noexcept
			{
				return m_ID >= other.m_ID;
			}
	};
}

namespace std
{
	template<>
	struct hash<kxf::LocallyUniqueID>
	{
		constexpr size_t operator()(const kxf::LocallyUniqueID& luid) const noexcept
		{
			return std::hash<uint64_t>()(luid.ToInt());
		}
	};
}
