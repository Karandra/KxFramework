#pragma once
#include "Common.h"
#include "kxf/General/ResourceID.h"
#include <variant>
#include <map>

namespace kxf
{
	enum class LocalizationItemKind
	{
		None = 0,

		SingleItem,
		MultipleItems,
		Plurals
	};
	enum class LocalizationItemQuantity
	{
		One,
		Few,
		Many,
		Other
	};

	enum class LocalizationItemFlag: uint32_t
	{
		None = 0,

		Translatable = 1 << 1,
		LengthRestricted = 1 << 2
	};
	KxDeclareFlagSet(LocalizationItemFlag);
}

namespace kxf
{
	class KX_API LocalizationItem final
	{
		public:
			using TSingleItem = String;
			using TMultipleItems = std::vector<TSingleItem>;
			using TPlurals = std::map<LocalizationItemQuantity, TSingleItem>;

		private:
			std::variant<void*, TSingleItem, TMultipleItems, TPlurals> m_Value;

			FlagSet<LocalizationItemFlag> m_Flags;
			int m_MaxLength = -1;

		public:
			LocalizationItem() = default;
			LocalizationItem(TSingleItem value, FlagSet<LocalizationItemFlag> flags = {}) noexcept
				:m_Flags(flags)
			{
				if (!value.IsEmpty())
				{
					m_Value = std::move(value);
				}
			}
			LocalizationItem(TMultipleItems items, FlagSet<LocalizationItemFlag> flags = {}) noexcept
				:m_Flags(flags)
			{
				if (!items.empty())
				{
					for (const auto& item: items)
					{
						if (item.IsEmpty())
						{
							return;
						}
					}
					m_Value = std::move(items);
				}
			}
			LocalizationItem(TPlurals items, FlagSet<LocalizationItemFlag> flags = {}) noexcept
				:m_Flags(flags)
			{
				if (!items.empty())
				{
					for (const auto& [id, item]: items)
					{
						if (item.IsEmpty())
						{
							return;
						}
					}
					m_Value = std::move(items);
				}
			}
			LocalizationItem(const LocalizationItem&) = default;
			LocalizationItem(LocalizationItem&&) noexcept = default;

		public:
			bool IsNull() const noexcept
			{
				if (m_Value.valueless_by_exception() || m_Value.index() == 0)
				{
					return true;
				}
				return GetItemCount() == 0;
			}
			size_t GetItemCount() const noexcept
			{
				if (auto value = std::get_if<TSingleItem>(&m_Value))
				{
					return !value->IsEmpty() ? 1 : 0;
				}
				else if (auto value = std::get_if<TMultipleItems>(&m_Value))
				{
					return value->size();
				}
				else if (auto value = std::get_if<TPlurals>(&m_Value))
				{
					return value->size();
				}
				return 0;
			}
			
			LocalizationItemKind GetKind() const noexcept
			{
				if (!m_Value.valueless_by_exception() && m_Value.index() != 0)
				{
					return static_cast<LocalizationItemKind>(m_Value.index());
				}
				return LocalizationItemKind::None;
			}
			FlagSet<LocalizationItemFlag> GetFlags() const noexcept
			{
				return m_Flags;
			}

			int GetMaxLength() const noexcept
			{
				return m_MaxLength;
			}
			void SetMaxLength(int maxLength) noexcept
			{
				maxLength = std::clamp(maxLength, -1, std::numeric_limits<int>::max());

				m_MaxLength = maxLength;
				m_Flags.Mod(LocalizationItemFlag::LengthRestricted, maxLength != -1);
			}

		public:
			const String& GetString(size_t index = 0) const noexcept
			{
				if (auto signleItem = std::get_if<TSingleItem>(&m_Value))
				{
					return *signleItem;
				}
				else if (auto items = std::get_if<TMultipleItems>(&m_Value); items && index < items->size())
				{
					return (*items)[index];
				}
				return NullString;
			}
			const String& GetPluralString(LocalizationItemQuantity quantity) const noexcept
			{
				if (auto items = std::get_if<TPlurals>(&m_Value))
				{
					if (auto it = items->find(quantity); it != items->end())
					{
						return it->second;
					}
				}
				return NullString;
			}

			template<class TFunc>
			size_t EnumStrings(TFunc&& func) const noexcept(std::is_nothrow_invocable_v<TFunc, const TSingleItem&>)
			{
				if (auto value = std::get_if<TSingleItem>(&m_Value))
				{
					if (!value->IsEmpty())
					{
						std::invoke(func, *value);
					}
				}
				else if (auto items = std::get_if<TMultipleItems>(&m_Value))
				{
					size_t count = 0;
					for (const TSingleItem& item: items)
					{
						count++;
						if (!std::invoke(func, item))
						{
							break;
						}
					}
					return count;
				}
				return 0;
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			operator const String&() const noexcept
			{
				return GetString();
			}
			const String& operator[](size_t index) const noexcept
			{
				return GetString(index);
			}

			bool operator==(const LocalizationItem& other) const noexcept
			{
				if (this != &other)
				{
					return m_Flags == other.m_Flags && m_MaxLength == other.m_MaxLength && m_Value == other.m_Value;
				}
				return true;
			}
			bool operator!=(const LocalizationItem& other) const noexcept
			{
				return !(*this == other);
			}

			LocalizationItem& operator=(const LocalizationItem&) = default;
			LocalizationItem& operator=(LocalizationItem&&) noexcept = default;
	};
}
