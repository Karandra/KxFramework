#pragma once
#include "Common.h"
#include "String.h"

namespace kxf
{
	enum class BinarySizeUnit
	{
		Auto = -1,

		Bytes = 0,
		KiloBytes = 1,
		MegaBytes = 2,
		GigaBytes = 3,
		TeraBytes = 4,
	};
	enum class BinarySizeFormat: uint32_t
	{
		None = 0,
		Fractional = 1 << 0,
		WithLabel = 1 << 1,
	};
	KxFlagSet_Declare(BinarySizeFormat);
}

namespace kxf
{
	class KX_API BinarySize final
	{
		public:
			static constexpr BinarySize FromBytes(int64_t bytes) noexcept
			{
				return bytes;
			}
			
			template<class T>
			static constexpr BinarySize FromKB(T value) noexcept
			{
				return FromBytes(value * 1024);
			}
			
			template<class T>
			static constexpr BinarySize FromMB(T value) noexcept
			{
				return FromKB(value * 1024);
			}
			
			template<class T>
			static constexpr BinarySize FromGB(T value) noexcept
			{
				return FromMB(value * 1024);
			}
			
			template<class T>
			static constexpr BinarySize FromTB(T value) noexcept
			{
				return FromGB(value * 1024);
			}

			template<class T>
			static constexpr BinarySize FromUnit(T value, BinarySizeUnit unit) noexcept
			{
				switch (unit)
				{
					case BinarySizeUnit::Bytes:
					{
						return FromBytes(value);
					}
					case BinarySizeUnit::KiloBytes:
					{
						return FromKB(value);
					}
					case BinarySizeUnit::MegaBytes:
					{
						return FromMB(value);
					}
					case BinarySizeUnit::GigaBytes:
					{
						return FromGB(value);
					}
					case BinarySizeUnit::TeraBytes:
					{
						return FromTB(value);
					}
				};
				return {};
			}

		private:
			int64_t m_Value = -1;

		private:
			template<class T> requires(std::is_integral_v<T>)
			constexpr T GetAs(int64_t value) const noexcept
			{
				return static_cast<T>(value / 1024);
			}

			template<class T> requires(std::is_floating_point_v<T>)
			constexpr T GetAs(int64_t value) const noexcept
			{
				return static_cast<T>(value / 1024.0);
			}

		public:
			constexpr BinarySize() noexcept = default;
			constexpr BinarySize(int64_t bytes) noexcept
				:m_Value(bytes)
			{
			}

		public:
			// Value testing
			constexpr bool IsNull() const noexcept
			{
				return m_Value == 0;
			}
			constexpr bool IsPositive() const noexcept
			{
				return m_Value > 0;
			}
			constexpr bool IsNegative() const noexcept
			{
				return m_Value < 0;
			}
			constexpr bool IsValid() const noexcept
			{
				return m_Value >= 0;
			}

			// Conversion
			template<class T = int64_t>
			constexpr T ToBytes() const noexcept
			{
				static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "invalid numeric type");

				return static_cast<T>(m_Value);
			}
			
			template<class T = int64_t>
			constexpr T ToKB() const noexcept
			{
				return GetAs<T>(m_Value);
			}
			
			template<class T = int64_t>
			constexpr T ToMB() const noexcept
			{
				return GetAs<T>(ToKB());
			}
			
			template<class T = int64_t>
			constexpr T ToGB() const noexcept
			{
				return GetAs<T>(ToMB());
			}
			
			template<class T = int64_t>
			constexpr T ToTB() const noexcept
			{
				return GetAs<T>(ToGB());
			}
			
			template<class T = int64_t>
			constexpr T ToUnit(BinarySizeUnit unit) const noexcept
			{
				switch (unit)
				{
					case BinarySizeUnit::Bytes:
					{
						return static_cast<T>(ToBytes());
					}
					case BinarySizeUnit::KiloBytes:
					{
						return ToKB<T>();
					}
					case BinarySizeUnit::MegaBytes:
					{
						return ToMB<T>();
					}
					case BinarySizeUnit::GigaBytes:
					{
						return ToGB<T>();
					}
					case BinarySizeUnit::TeraBytes:
					{
						return ToTB<T>();
					}
				};
				return -1;
			}

			// Formatting
			String ToString(BinarySizeUnit unit = BinarySizeUnit::Auto, FlagSet<BinarySizeFormat> format = {}, int precision = -1) const;

		public:
			// Comparison
			constexpr auto operator<=>(const BinarySize&) const noexcept = default;

			// Arithmetics
			constexpr BinarySize operator+(const BinarySize& other) const noexcept
			{
				return m_Value + other.m_Value;
			}
			constexpr BinarySize operator-(const BinarySize& other) const noexcept
			{
				return m_Value - other.m_Value;
			}
			constexpr BinarySize operator*(int64_t n) const noexcept
			{
				return m_Value * n;
			}
			constexpr BinarySize operator*(double n) const noexcept
			{
				return m_Value * n;
			}
			constexpr BinarySize operator/(int64_t n) const noexcept
			{
				return m_Value / n;
			}
			constexpr BinarySize operator/(double n) const noexcept
			{
				return m_Value / n;
			}

			constexpr BinarySize& operator+=(const BinarySize& other) noexcept
			{
				m_Value += other.m_Value;
				return *this;
			}
			constexpr BinarySize& operator-=(const BinarySize& other) noexcept
			{
				m_Value -= other.m_Value;
				return *this;
			}
			constexpr BinarySize& operator*=(int64_t n) noexcept
			{
				m_Value *= n;
				return *this;
			}
			constexpr BinarySize& operator*=(double n) noexcept
			{
				m_Value *= n;
				return *this;
			}
			constexpr BinarySize& operator/=(int64_t n) noexcept
			{
				m_Value /= n;
				return *this;
			}
			constexpr BinarySize& operator/=(double n) noexcept
			{
				m_Value /= n;
				return *this;
			}

			// Validity test
			constexpr explicit operator bool() const noexcept
			{
				return IsValid();
			}
			constexpr bool operator!() const noexcept
			{
				return !IsValid();
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<BinarySize> final
	{
		uint64_t Serialize(IOutputStream& stream, const BinarySize& value) const
		{
			return Serialization::WriteObject(stream, value.ToBytes());
		}
		uint64_t Deserialize(IInputStream& stream, BinarySize& value) const
		{
			int64_t buffer = 0;
			auto read = Serialization::ReadObject(stream, buffer);
			value = BinarySize::FromBytes(buffer);

			return read;
		}
	};
}

namespace kxf
{
	inline double GetSizeRatio(BinarySize smallerSize, BinarySize largerSize) noexcept
	{
		if (smallerSize && largerSize && !largerSize.IsNull())
		{
			return smallerSize.ToBytes<double>() / largerSize.ToBytes<double>();
		}
		return -1;
	}
}
