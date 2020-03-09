#pragma once
#include <optional>
#include "KxFramework/KxStringUtility.h"

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
				for (const TItem& item: TDerived::ms_Index)
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
					const TInt index = static_cast<TInt>(value);
					if constexpr(std::is_signed_v<TInt>)
					{
						if (index >= 0 && index < static_cast<TInt>(GetEnumSize()))
						{
							return &TDerived::ms_Index[index];
						}
					}
					else
					{
						if (index < GetEnumSize())
						{
							return &TDerived::ms_Index[index];
						}
					}
				}
				else
				{
					for (const TItem& item: TDerived::ms_Index)
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
			constexpr static size_t GetEnumSize()
			{
				return std::size(TDerived::ms_Index);
			}

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
	
			template<class T> constexpr static T ToAnyString(TEnum value)
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
						return T(name.data(), name.size());
					}
				}
				return T {};
			}
			template<class T> constexpr static std::optional<T> TryToAnyString(TEnum value)
			{
				static_assert(std::is_constructible_v<T, TString>, "T can not be constructed from TString");

				const TItem* item = FindByValue(value);
				if (item)
				{
					const TString& name = item->GetName();
					if constexpr(std::is_pointer_v<TString>)
					{
						return T(name);
					}
					else
					{
						return std::optional<T>(T(name.data(), name.size()));
					}
				}
				return std::nullopt;
			}
			
			static TEnum FromOrExpression(const TString& string, TEnum initialValue = static_cast<TEnum>(0))
			{
				using namespace KxUtility::String;

				TInt intValue = static_cast<TInt>(initialValue);
				SplitBySeparator(string, wxS('|'), [&intValue](wxStringView value)
				{
					const TItem* item = FindByName(TString(value.data(), value.size()));
					if (item)
					{
						intValue |= static_cast<TInt>(item->GetValue());
					}
					return true;
				});
				return static_cast<TEnum>(intValue);
			}
			static wxString ToOrExpression(TEnum value)
			{
				wxString stringExpr;
				for (const TItem& item: TDerived::ms_Index)
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
			Value(TInt value):
				m_Value(static_cast<TEnum>(value))
			{
			}

		public:
			constexpr bool IsDefault() const
			{
				return m_Value == t_DefaultValue;
			}
			constexpr bool FromString(const TString& string, TEnum defaultValue = t_DefaultValue)
			{
				TEnumOpt value = TDefinition::TryFromString(string);
				if (value)
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
			
			bool FromOrExpression(const TString& string, TEnum initialValue = static_cast<TEnum>(0))
			{
				m_Value = TDefinition::FromOrExpression(string, initialValue);
				return !IsDefault();
			}
			wxString ToOrExpression() const
			{
				return TDefinition::ToOrExpression(m_Value);
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
				return GetValue();
			}
			operator TInt() const
			{
				return GetInt();
			}
			
			Value& operator=(TEnum value)
			{
				m_Value = value;
				return *this;
			}
			Value& operator=(TInt value)
			{
				m_Value = static_cast<TEnum>(value);
				return *this;
			}

			bool operator==(const Value& other) const
			{
				return other.m_Value == m_Value;
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

			bool operator==(TInt value) const
			{
				return static_cast<TInt>(m_Value) == value;
			}
			bool operator!=(TInt value) const
			{
				return static_cast<TInt>(m_Value) != value;
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
				return (static_cast<TInt>(m_Value) & static_cast<TInt>(value)) == static_cast<TInt>(value);
			}
			bool HasAnyFlag(TEnum valueCombo) const
			{
				return static_cast<TInt>(m_Value) & static_cast<TInt>(valueCombo);
			}
	};
}
