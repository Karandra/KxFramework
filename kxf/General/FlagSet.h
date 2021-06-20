#pragma once
#include "kxf/Serialization/BinarySerializer.h"
#include "kxf/Utility/TypeTraits.h"
#include <utility>
#include <format>
#include <type_traits>

namespace kxf
{
	template<class T>
	struct IsFlagSet: std::false_type {};

	template<class T>
	inline constexpr bool IsFlagSet_v = IsFlagSet<T>::value;

	#define KxFlagSet_Declare(T)	\
		template<>	\
		struct IsFlagSet<T>: std::true_type	\
		{	\
			static_assert(std::is_enum_v<T>, "enum type required");	\
			static_assert(std::is_unsigned_v<std::underlying_type_t<T>>, "underlying type of the enum must be unsigned integer");	\
		}
}

namespace kxf
{
	template<class TInt, class TEnum> requires(std::is_enum_v<TEnum>)
	constexpr TInt ToInt(TEnum value) noexcept
	{
		return static_cast<TInt>(value);
	}

	template<class TEnum, class TInt = std::underlying_type_t<TEnum>> requires(std::is_enum_v<TEnum>)
	constexpr TInt ToInt(TEnum value) noexcept
	{
		return static_cast<TInt>(value);
	}

	template<class TEnum, class TInt> requires(std::is_enum_v<TEnum>)
	constexpr TEnum FromInt(TInt value) noexcept
	{
		return static_cast<TEnum>(value);
	}

	template<class TEnum> requires(IsFlagSet_v<TEnum>)
	constexpr TEnum operator|(TEnum left, TEnum right) noexcept
	{
		using Tx = std::underlying_type_t<TEnum>;
		return static_cast<TEnum>(static_cast<Tx>(left) | static_cast<Tx>(right));
	}

	template<class TEnum, class... Args> requires((IsFlagSet_v<std::remove_const_t<std::remove_reference_t<Args>>> && ...))
	constexpr TEnum CombineFlags(Args&&... arg) noexcept
	{
		return static_cast<TEnum>((static_cast<std::underlying_type_t<std::remove_const_t<std::remove_reference_t<Args>>>>(arg) | ...));
	}

	template<class TEnum> requires(std::is_enum_v<TEnum>)
	constexpr auto FlagSetValue(size_t index) noexcept
	{
		return static_cast<std::underlying_type_t<TEnum>>(1) << index;
	}
}

namespace kxf
{
	template<class TEnum_>
	class FlagSet final
	{
		public:
			using TEnum = TEnum_;
			using TInt = Utility::UnderlyingTypeEx_t<TEnum_>;

		private:
			TEnum m_Value = static_cast<TEnum>(0);

		public:
			constexpr FlagSet() noexcept = default;
			constexpr FlagSet(TEnum values) noexcept
				:m_Value(values)
			{
			}
			constexpr FlagSet(const FlagSet&) noexcept = default;
			constexpr FlagSet(FlagSet&&) noexcept = default;

		public:
			constexpr bool IsNull() const noexcept
			{
				return ToInt() == 0;
			}
			constexpr FlagSet& Clear() noexcept
			{
				FromInt(0);
				return *this;
			}
			constexpr FlagSet Clone() const noexcept
			{
				return *this;
			}
			
			constexpr TEnum GetValue() const noexcept
			{
				return m_Value;
			}
			constexpr FlagSet& SetValue(TEnum value) noexcept
			{
				m_Value = value;
				return *this;
			}
			constexpr TEnum operator*() const noexcept
			{
				return m_Value;
			}

			template<class T = TInt> requires(std::is_integral_v<T> || std::is_enum_v<T>)
			constexpr T ToInt() const noexcept
			{
				return static_cast<T>(m_Value);
			}

			constexpr FlagSet& FromInt(TInt value) noexcept
			{
				m_Value = static_cast<TEnum>(value);
				return *this;
			}

			constexpr bool Equals(FlagSet other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			constexpr bool Contains(FlagSet other) const noexcept
			{
				return (ToInt() & other.ToInt()) == other.ToInt();
			}
			
			constexpr FlagSet& Add(FlagSet other) noexcept
			{
				FromInt(ToInt() | other.ToInt());
				return *this;
			}
			constexpr FlagSet& Add(FlagSet other, bool condition) noexcept
			{
				if (condition)
				{
					Add(other);
				}
				return *this;
			}
			
			constexpr FlagSet& Remove(FlagSet other) noexcept
			{
				FromInt(ToInt() & ~other.ToInt());
				return *this;
			}
			constexpr FlagSet& Remove(FlagSet other, bool condition) noexcept
			{
				if (condition)
				{
					Remove(other);
				}
				return *this;
			}
			
			constexpr FlagSet& Toggle(FlagSet other) noexcept
			{
				FromInt(ToInt() ^ other.ToInt());
				return *this;
			}
			constexpr FlagSet& Mod(FlagSet other, bool enable) noexcept
			{
				if (enable)
				{
					Add(other);
				}
				else
				{
					Remove(other);
				}
				return *this;
			}

			constexpr TEnum ExtractConsecutive(FlagSet mask) const noexcept
			{
				return static_cast<TEnum>(ToInt() & mask.ToInt());
			}
			constexpr TEnum ExtractConsecutiveRange(size_t last) const noexcept
			{
				return ExtractConsecutive(static_cast<TInt>(1) << (last + 1) - 1);
			}
			constexpr TEnum ExtractConsecutiveRange(size_t first, size_t last) const noexcept
			{
				TInt begin = first != 0 ? static_cast<TInt>(1) << (first + 1) - 1 : static_cast<TInt>(0);
				TInt end = static_cast<TInt>(1) << (last + 1) - 1;

				return ExtractConsecutive(begin ^ end);
			}

		public:
			constexpr operator bool() const noexcept
			{
				static_assert(!std::is_integral_v<TEnum>, "cannot use 'operator bool' for flag sets instantiated for integers");

				return !IsNull();
			}
			constexpr bool operator!() const noexcept
			{
				return IsNull();
			}

			constexpr auto operator<=>(const FlagSet&) const noexcept = default;

			constexpr FlagSet& operator=(const FlagSet&) noexcept = default;
			constexpr FlagSet& operator=(FlagSet&&) noexcept = default;

			constexpr FlagSet& operator|=(FlagSet other) noexcept
			{
				FromInt(ToInt() | other.ToInt());
				return *this;
			}
			constexpr FlagSet& operator&=(FlagSet other) noexcept
			{
				FromInt(ToInt() & other.ToInt());
				return *this;
			}
			constexpr FlagSet& operator^=(FlagSet other) noexcept
			{
				FromInt(ToInt() ^ other.ToInt());
				return *this;
			}

			constexpr FlagSet operator|(FlagSet other) const noexcept
			{
				auto clone = Clone();
				clone |= other;
				return clone;
			}
			constexpr FlagSet operator&(FlagSet other) const noexcept
			{
				auto clone = Clone();
				clone &= other;
				return clone;
			}
			constexpr FlagSet operator^(TEnum other) const noexcept
			{
				auto clone = Clone();
				clone ^= other;
				return clone;
			}
			constexpr FlagSet operator~() const noexcept
			{
				return Clone().FromInt(~ToInt());
			}
	};
}

namespace kxf
{
	template<class T>
	struct BinarySerializer<FlagSet<T>> final
	{
		uint64_t Serialize(IOutputStream& stream, const FlagSet<T>& value) const
		{
			return Serialization::WriteObject(stream, value.ToInt());
		}
		uint64_t Deserialize(IInputStream& stream, FlagSet<T>& value) const
		{
			typename FlagSet<T>::TInt intValue = 0;
			auto read = Serialization::ReadObject(stream, intValue);
			value.FromInt(intValue);

			return read;
		}
	};
}

namespace std
{
	template<class T>
	struct hash<kxf::FlagSet<T>> final
	{
		constexpr size_t operator()(const kxf::FlagSet<T>& flagSet) const noexcept
		{
			return std::hash<typename kxf::FlagSet<T>::TInt>()(flagSet.ToInt());
		}
	};

	template<class TEnum, class TChar>
	struct formatter<kxf::FlagSet<TEnum>, TChar>: formatter<typename kxf::FlagSet<TEnum>::TInt, TChar>
	{
		template<class TFormatContext>
		auto format(const kxf::FlagSet<TEnum>& value, TFormatContext& formatContext)
		{
			using Tx = typename kxf::FlagSet<TEnum>::TInt;
			return formatter<Tx, TChar>::format(value.ToInt(), formatContext);
		}
	};
}
