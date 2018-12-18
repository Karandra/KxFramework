#pragma once
#include <optional>

namespace KxIndexedEnum
{
	template <class TEnum, class TString>
	class StoreItem
	{
		private:
			const TString m_Name;
			const TEnum m_Value;

		public:
			constexpr StoreItem(TEnum value, TString&& name)
				:m_Name(std::move(name)), m_Value(value)
			{
			}
			constexpr StoreItem(TEnum value, const TString& name)
				:m_Name(name), m_Value(value)
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

	template <class T_Derived, class T_Enum, class T_String, bool t_DirectAccess = false>
	class Definition
	{
		public:
			using TDerived = T_Derived;

			using TEnum = T_Enum;
			using TInt = typename std::underlying_type<TEnum>::type;
			using TString = T_String;
			using TItem = StoreItem<TEnum, TString>;

			using TIntOpt = std::optional<TInt>;
			using TEnumOpt = std::optional<TEnum>;
			using TStringOpt = std::optional<TString>;

		protected:
			constexpr static const TItem* FindByName(const TString& string)
			{
				for (const TItem& item: TDerived::m_Index)
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
				if constexpr(t_DirectAccess)
				{
					return &TDerived::m_Index[static_cast<size_t>(value)];
				}
				else
				{
					for (const TItem& item: TDerived::m_Index)
					{
						if (item.GetValue() == value)
						{
							return &item;
						}
					}
					return nullptr;
				}
			}

		public:
			constexpr static TEnum FromString(const TString& string, TEnum defaultValue)
			{
				const TItem* item = FindByName(string);
				return item ? item->GetValue() : defaultValue;
			}
			constexpr static TEnumOpt TryFromString(const TString& string)
			{
				const TItem* item = FindByName(string);
				return item ? TEnumOpt(item->GetValue()) : std::nullopt;
			}
			
			constexpr static TInt FromStringToInt(const TString& string, TInt defaultValue)
			{
				return static_cast<TInt>(ToEnum(string, defaultValue));
			}
			constexpr static TIntOpt TryFromStringToInt(const TString& string)
			{
				const TItem* item = FindByName(string);
				return item ? TIntOpt(static_cast<TInt>(item->GetValue())) : std::nullopt;
			}

			constexpr static TString ToString(TEnum value)
			{
				const TItem* item = FindByValue(value);
				return item ? item->GetName() : TString{};
			}
			constexpr static TStringOpt TryToString(TEnum value)
			{
				const TItem* item = FindByValue(value);
				return item ? TStringOpt(item->GetName()) : std::nullopt;
			}
	};
}

namespace KxIndexedEnum
{
	template <class T_Definition, typename T_Definition::TEnum t_DefaultValue>
	class Value
	{
		public:
			using TDefinition = typename T_Definition;

			using TInt = typename TDefinition::TInt;
			using TEnum = typename TDefinition::TEnum;
			using TString = typename TDefinition::TString;

			using TIntOpt = typename TDefinition::TIntOpt;
			using TEnumOpt = typename TDefinition::TEnumOpt;
			using TStringOpt = typename TDefinition::TStringOpt;

		private:
			TEnum m_Value = t_DefaultValue;

		public:
			Value() = default;
			Value(TEnum value):
				m_Value(value)
			{
			}

		public:
			constexpr bool FromString(const TString& string, TEnum defaultValue = t_DefaultValue)
			{
				TEnumOpt value = TDefinition::TryFromString(string, t_DefaultValue);
				if (value.has_value())
				{
					m_Value = value.get_value();
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

			TDefinition GetDefinition() const
			{
				return TDefinition();
			}
			TEnum GetValue() const
			{
				return m_Value;
			}
			TInt GetInt() const
			{
				return static_cast<TInt>(m_Value);
			}
			operator TEnum() const
			{
				return m_Value;
			}
			
			Value& operator=(TEnum value)
			{
				m_Value = value;
				return *this;
			}

			bool operator==(const Value& other) const
			{
				return other.m_value == m_Value;
			}
			bool operator!=(const Value& other) const
			{
				return !(*this == other);
			}

			bool operator==(TEnum value) const
			{
				return m_Value == value;
			}
			bool operator!=(TEnum value) const
			{
				return m_Value != value;
			}

		public:
			void AddFlag(TEnum value)
			{
				*reinterpret_cast<TInt*>(&m_Value) |= static_cast<TInt>(value);
			}
			void RemoveFlag(TEnum value)
			{
				*reinterpret_cast<TInt*>(&m_Value) &= ~static_cast<TInt>(value);
			}
			bool HasFlag(TEnum value) const
			{
				return (m_Value & value) == value;
			}
			bool HasAnyFlag(TEnum valueCombo) const
			{
				return (m_Value & valueCombo) != 0;
			}
	};
}
