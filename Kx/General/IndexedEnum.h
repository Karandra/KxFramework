#pragma once
#include "Common.h"
#include "String.h"
#include "Kx/Utility/Common.h"

namespace KxFramework
{
	template<class TEnum, class TString>
	class IndexedEnumItem final
	{
		private:
			const TString m_Name;
			const TEnum m_Value;

		public:
			constexpr IndexedEnumItem(TEnum value, TString&& name)
				:m_Name(std::forward<TString>(name)), m_Value(value)
			{
			}

		public:
			constexpr const TString& GetName() const
			{
				return m_Name;
			}
			constexpr TEnum GetValue() const
			{
				return m_Value;
			}
	};

	template<class TDerived_, class TEnum_, class TString_, bool t_DirectAccess = false>
	class IndexedEnumDefinition
	{
		public:
			using TDerived = TDerived_;

			using TEnum = TEnum_;
			using TInt = typename std::underlying_type<TEnum>::type;
			using TString = TString_;
			using TItem = IndexedEnumItem<TEnum, TString>;

		protected:
			constexpr static const auto& GetItems() noexcept
			{
				return TDerived::Items;
			}
			constexpr static TEnum GetNullValue() noexcept
			{
				return static_cast<TEnum>(0);
			}

			constexpr static const TItem* FindByName(const TString& string)
			{
				for (const TItem& item: GetItems())
				{
					if (item.GetName() == string)
					{
						return &item;
					}
				}
				return nullptr;
			}
			constexpr static const TItem* FindByValue(TEnum value)
			{
				const auto& items = GetItems();

				if constexpr(t_DirectAccess)
				{
					const TInt index = static_cast<TInt>(value);
					if constexpr(std::is_signed_v<TInt>)
					{
						if (index >= 0 && index < static_cast<TInt>(GetItemCount()))
						{
							return &items[index];
						}
					}
					else
					{
						if (index < GetItemCount())
						{
							return &items[index];
						}
					}
				}
				else
				{
					for (const TItem& item: items)
					{
						if (item.GetValue() == value)
						{
							return &item;
						}
					}
				}
				return nullptr;
			}
			
		public:
			constexpr IndexedEnumDefinition() noexcept = delete;

		public:
			constexpr static size_t GetItemCount()
			{
				return std::size(GetItems());
			}

			constexpr static TEnum FromString(const TString& string, TEnum defaultValue)
			{
				const TItem* item = FindByName(string);
				return item ? item->GetValue() : defaultValue;
			}
			constexpr static std::optional<TEnum> TryFromString(const TString& string)
			{
				if (const TItem* item = FindByName(string))
				{
					return item->GetValue();
				}
				return {};
			}
			
			constexpr static TInt FromStringToInt(const TString& string, TInt defaultValue)
			{
				return static_cast<TInt>(ToEnum(string, defaultValue));
			}
			constexpr static std::optional<TInt> TryFromStringToInt(const TString& string)
			{
				if (const TItem* item = FindByName(string))
				{
					return static_cast<TInt>(item->GetValue());
				}
				return {};
			}

			constexpr static TString ToString(TEnum value)
			{
				const TItem* item = FindByValue(value);
				return item ? item->GetName() : TString{};
			}
			constexpr static std::optional<TString> TryToString(TEnum value)
			{
				if (const TItem* item = FindByValue(value))
				{
					return item;
				}
				return {};
			}
			
			template<class T>
			constexpr static T ToAnyString(TEnum value)
			{
				static_assert(std::is_constructible_v<T, TString>, "T can not be constructed from TString");

				const TItem* item = FindByValue(value);
				if (item)
				{
					const TString& name = item->GetName();
					if constexpr(std::is_pointer_v<TString>)
					{
						return name;
					}
					else
					{
						return {name.data(), name.size()};
					}
				}
				return {};
			}
			
			template<class T>
			constexpr static std::optional<T> TryToAnyString(TEnum value)
			{
				static_assert(std::is_constructible_v<T, TString>, "T can not be constructed from TString");

				if (const TItem* item = FindByValue(value))
				{
					const TString& name = item->GetName();
					if constexpr(std::is_pointer_v<TString>)
					{
						return T{name};
					}
					else
					{
						return T{name.data(), name.size()};
					}
				}
				return {};
			}
			
			template<class TFunc>
			constexpr static size_t EnumItems(TFunc&& func)
			{
				size_t count = 0;
				for (const TItem& item: GetItems())
				{
					count++;
					if (!std::invoke(func, item))
					{
						break;
					}
				}
				return count;
			}

			static TEnum FromOrExpression(const TString& string, TEnum initialValue = GetNullValue())
			{
				TInt intValue = static_cast<TInt>(initialValue);
				String::SplitBySeparator(string, wxS('|'), [&intValue](StringView value)
				{
					if (const TItem* item = FindByName(TString(value.data(), value.size())))
					{
						intValue |= static_cast<TInt>(item->GetValue());
					}
					return true;
				});
				return static_cast<TEnum>(intValue);
			}
			static String ToOrExpression(TEnum value)
			{
				String stringExpr;
				for (const TItem& item: GetItems())
				{
					if (static_cast<TInt>(item.GetValue()) & static_cast<TInt>(value))
					{
						if (!stringExpr.empty())
						{
							stringExpr += wxS('|');
						}
						stringExpr += item.GetName();
					}
				}
				return stringExpr;
			}
	};
}

namespace KxFramework
{
	template<class TDefinition_, typename TDefinition_::TEnum t_DefaultValue>
	class IndexedEnumValue
	{
		public:
			using TDefinition = typename TDefinition_;

			using TInt = typename TDefinition::TInt;
			using TEnum = typename TDefinition::TEnum;
			using TString = typename TDefinition::TString;

		private:
			TEnum m_Value = t_DefaultValue;

		public:
			constexpr IndexedEnumValue() noexcept = default;
			constexpr IndexedEnumValue(TEnum value) noexcept
				:m_Value(value)
			{
			}
			constexpr IndexedEnumValue(TInt value) noexcept
				:m_Value(static_cast<TEnum>(value))
			{
			}

		public:
			constexpr bool IsDefault() const noexcept
			{
				return m_Value == t_DefaultValue;
			}
			constexpr bool FromString(const TString& string, TEnum defaultValue = t_DefaultValue)
			{
				if (auto value = TDefinition::TryFromString(string))
				{
					m_Value = *value;
					return true;
				}
				else
				{
					m_Value = defaultValue;
					return false;
				}
			}
			constexpr TString ToString() const
			{
				return TDefinition::ToString(m_Value);
			}
			
			template<class T>
			constexpr T ToAnyString(TEnum value) const
			{
				return TDefinition::ToAnyString(m_Value);
			}

			bool FromOrExpression(const TString& string, TEnum initialValue = TDefinition::GetNullValue())
			{
				m_Value = TDefinition::FromOrExpression(string, initialValue);
				return !IsDefault();
			}
			String ToOrExpression() const
			{
				return TDefinition::ToOrExpression(m_Value);
			}

			constexpr IndexedEnumValue& AddFlag(TEnum value) noexcept
			{
				Utility::AddFlagRef(m_Value, value);
				return *this;
			}
			constexpr IndexedEnumValue& RemoveFlag(TEnum value) noexcept
			{
				Utility::RemoveFlagRef(m_Value, value);
				return *this;
			}
			constexpr IndexedEnumValue& ModFlag(TEnum value, bool condition) noexcept
			{
				Utility::ModFlagRef(m_Value, value, condition);
				return *this;
			}
			constexpr bool HasFlag(TEnum value) const noexcept
			{
				Utility::HasFlag(m_Value, value);
			}
			constexpr bool HasSpecifiedFlagOnly(TEnum value) const noexcept
			{
				return static_cast<TInt>(m_Value) & static_cast<TInt>(value) == static_cast<TInt>(value);
			}

			constexpr TEnum GetValue() const noexcept
			{
				return m_Value;
			}
			constexpr IndexedEnumValue& SetValue(TEnum value) noexcept
			{
				m_Value = value;
				return *this;
			}

			constexpr TInt ToInt() const noexcept
			{
				return static_cast<TInt>(m_Value);
			}
			constexpr bool IsInRange(TInt min, TInt max) const noexcept
			{
				return ToInt() == std::clamp(ToInt(), min, max);
			}

		public:
			constexpr bool operator==(const IndexedEnumValue& other) const noexcept
			{
				return other.m_Value == m_Value;
			}
			constexpr bool operator!=(const IndexedEnumValue& other) const noexcept
			{
				return !(*this == other);
			}

			constexpr bool operator==(TEnum value) const noexcept
			{
				return m_Value == value;
			}
			constexpr bool operator!=(TEnum value) const noexcept
			{
				return m_Value != value;
			}

			constexpr bool operator==(TInt value) const noexcept
			{
				return ToInt() == value;
			}
			constexpr bool operator!=(TInt value) const noexcept
			{
				return ToInt() != value;
			}

			constexpr operator TEnum() const noexcept
			{
				return GetValue();
			}
	};
}
