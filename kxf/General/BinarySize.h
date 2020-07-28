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
	KxDeclareFlagSet(BinarySizeFormat);
}

namespace kxf
{
	class KX_API BinarySize final
	{
		public:
			using SizeType = int64_t;

		public:
			static constexpr BinarySize FromBytes(SizeType bytes) noexcept
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
			SizeType m_Value = -1;

		private:
			template<class T>
			constexpr T GetAs(SizeType value) const noexcept
			{
				if constexpr(std::is_floating_point_v<T>)
				{
					return static_cast<T>(value / 1024.0);
				}
				else if constexpr(std::is_integral_v<T>)
				{
					return static_cast<T>(value / 1024);
				}
				else
				{
					static_assert(false, "invalid numeric type");
				}
			}

		public:
			constexpr BinarySize() noexcept = default;
			constexpr BinarySize(SizeType bytes) noexcept
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
			template<class T = SizeType>
			constexpr T GetBytes() const noexcept
			{
				static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "invalid numeric type");

				return static_cast<T>(m_Value);
			}
			
			template<class T = SizeType>
			constexpr T GetKB() const noexcept
			{
				return GetAs<T>(m_Value);
			}
			
			template<class T = SizeType>
			constexpr T GetMB() const noexcept
			{
				return GetAs<T>(GetKB());
			}
			
			template<class T = SizeType>
			constexpr T GetGB() const noexcept
			{
				return GetAs<T>(GetMB());
			}
			
			template<class T = SizeType>
			constexpr T GetTB() const noexcept
			{
				return GetAs<T>(GetGB());
			}
			
			template<class T = SizeType>
			constexpr T GetAsUnit(BinarySizeUnit unit) const noexcept
			{
				switch (unit)
				{
					case BinarySizeUnit::Bytes:
					{
						return static_cast<T>(GetBytes());
					}
					case BinarySizeUnit::KiloBytes:
					{
						return GetKB<T>();
					}
					case BinarySizeUnit::MegaBytes:
					{
						return GetMB<T>();
					}
					case BinarySizeUnit::GigaBytes:
					{
						return GetGB<T>();
					}
					case BinarySizeUnit::TeraBytes:
					{
						return GetTB<T>();
					}
				};
				return -1;
			}

			// Formatting
			String Format(BinarySizeUnit unit = BinarySizeUnit::Auto, FlagSet<BinarySizeFormat> format = {}, int precision = -1) const;

		public:
			// Comparison
			constexpr bool operator==(const BinarySize& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			constexpr bool operator!=(const BinarySize& other) const noexcept
			{
				return m_Value != other.m_Value;
			}
			constexpr bool operator<(const BinarySize& other) const noexcept
			{
				return m_Value < other.m_Value;
			}
			constexpr bool operator<=(const BinarySize& other) const noexcept
			{
				return m_Value <= other.m_Value;
			}
			constexpr bool operator>(const BinarySize& other) const noexcept
			{
				return m_Value > other.m_Value;
			}
			constexpr bool operator>=(const BinarySize& other) const noexcept
			{
				return m_Value >= other.m_Value;
			}

			// Arithmetics
			constexpr BinarySize operator+(const BinarySize& other) const noexcept
			{
				return m_Value + other.m_Value;
			}
			constexpr BinarySize operator-(const BinarySize& other) const noexcept
			{
				return m_Value - other.m_Value;
			}
			constexpr BinarySize operator*(SizeType n) const noexcept
			{
				return m_Value * n;
			}
			constexpr BinarySize operator*(double n) const noexcept
			{
				return m_Value * n;
			}
			constexpr BinarySize operator/(SizeType n) const noexcept
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
			constexpr BinarySize& operator*=(SizeType n) noexcept
			{
				m_Value *= n;
				return *this;
			}
			constexpr BinarySize& operator*=(double n) noexcept
			{
				m_Value *= n;
				return *this;
			}
			constexpr BinarySize& operator/=(SizeType n) noexcept
			{
				m_Value /= n;
				return *this;
			}
			constexpr BinarySize& operator/=(double n) noexcept
			{
				m_Value /= n;
				return *this;
			}

			// Validity test (see  'IsValid')
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
	inline double GetSizeRatio(BinarySize smallerSize, BinarySize largerSize) noexcept
	{
		if (smallerSize && largerSize && !largerSize.IsNull())
		{
			return smallerSize.GetBytes<double>() / largerSize.GetBytes<double>();
		}
		return -1;
	}
}
