#pragma once
#include "Common.h"
#include "String.h"
#include <variant>

namespace kxf
{
	class KX_API ResourceID final
	{
		private:
			std::variant<int, String> m_ID;

		public:
			ResourceID() noexcept = default;
			ResourceID(int id) noexcept
				:m_ID(id)
			{
			}

			template<class T, class = std::enable_if_t<std::is_enum_v<T>>>
			ResourceID(T id) noexcept
				:m_ID(static_cast<int>(id))
			{
			}

			ResourceID(const ResourceID&) = default;
			ResourceID(ResourceID&&) noexcept = default;

		public:
			bool IsNull() const noexcept
			{
				if (m_ID.valueless_by_exception())
				{
					return true;
				}
				else if (auto value = std::get_if<int>(&m_ID))
				{
					return *value == 0;
				}
				else if (auto value = std::get_if<String>(&m_ID))
				{
					return value->IsEmpty();
				}
				return false;
			}

			int GetInt() const noexcept
			{
				if (auto value = std::get_if<int>(&m_ID))
				{
					return *value;
				}
				return 0;
			}

			template<class T, class = std::enable_if_t<std::is_enum_v<T>>>
			T GetInt() const noexcept
			{
				return static_cast<T>(GetInt());
			}

			const String& GetString() const& noexcept
			{
				if (auto value = std::get_if<String>(&m_ID))
				{
					return *value;
				}
				return NullString;
			}
			String GetString() && noexcept
			{
				if (auto value = std::get_if<String>(&m_ID))
				{
					return std::move(*value);
				}
				return {};
			}

		public:
			explicit operator bool() const
			{
				return !IsNull();
			}
			bool operator!() const
			{
				return IsNull();
			}

			bool operator==(const ResourceID& other) const noexcept
			{
				return this == &other || m_ID == other.m_ID;
			}
			bool operator!=(const ResourceID& other) const noexcept
			{
				return !(*this == other);
			}

			ResourceID& operator=(const ResourceID&) = default;
			ResourceID& operator=(ResourceID&&) noexcept = default;
	};
}
