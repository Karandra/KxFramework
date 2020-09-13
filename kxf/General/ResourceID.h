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

			template<class T, class = std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>>>
			ResourceID(T id) noexcept
				:m_ID(static_cast<int>(id))
			{
			}

			ResourceID(String id) noexcept
			{
				if (!id.IsEmptyOrWhitespace())
				{
					m_ID = std::move(id);
				}
			}
			ResourceID(const char* id) noexcept
				:ResourceID(String(id))
			{
			}
			ResourceID(const wchar_t* id) noexcept
				:ResourceID(String(id))
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
			size_t GetHash() const noexcept
			{
				if (auto value = std::get_if<int>(&m_ID))
				{
					return std::hash<int>()(*value);
				}
				else if (auto value = std::get_if<String>(&m_ID))
				{
					return std::hash<String>()(*value);
				}
				return 0;
			}

			// Integer
			template<class T = int, class = std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>>>
			T GetInt() const noexcept
			{
				if (auto value = std::get_if<int>(&m_ID))
				{
					return static_cast<T>(*value);
				}
				return 0;
			}

			template<class T = int, class = std::enable_if_t<std::is_integral_v<T> || std::is_enum_v<T>>>
			std::optional<T> QueryInt() const noexcept
			{
				if (auto value = std::get_if<int>(&m_ID))
				{
					return static_cast<T>(*value);
				}
				return {};
			}

			// String
			String GetString() const& noexcept
			{
				if (auto value = std::get_if<String>(&m_ID))
				{
					return *value;
				}
				return {};
			}
			String GetString() && noexcept
			{
				if (auto value = std::get_if<String>(&m_ID))
				{
					return std::move(*value);
				}
				return {};
			}

			std::optional<String> QueryString() const&
			{
				if (auto value = std::get_if<String>(&m_ID))
				{
					return *value;
				}
				return {};
			}
			std::optional<String> QueryString() && noexcept
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

namespace std
{
	template<>
	struct hash<kxf::ResourceID> final
	{
		size_t operator()(const kxf::ResourceID& id) const noexcept
		{
			return id.GetHash();
		}
	};
}
