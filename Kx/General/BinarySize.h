#pragma once
#include "Common.h"

namespace KxFramework
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
	enum class BinarySizeFormat
	{
		None = 0,
		Fractional = 1 << 0,
		WithLabel = 1 << 0,
	};
}
namespace KxEnumClassOperations
{
	KxAllowEnumCastOp(KxFramework::BinarySizeUnit);
	KxImplementEnum(KxFramework::BinarySizeFormat);
}

namespace KxFramework
{
	class KX_API BinarySize final
	{
		public:
			static constexpr BinarySize FromBytes(int64_t bytes)
			{
				return bytes;
			}
			
			template<class T>
			static constexpr BinarySize FromKB(T value)
			{
				return FromBytes(value * 1024);
			}
			
			template<class T>
			static constexpr BinarySize FromMB(T value)
			{
				return FromKB(value * 1024);
			}
			
			template<class T>
			static constexpr BinarySize FromGB(T value)
			{
				return FromMB(value * 1024);
			}
			
			template<class T>
			static constexpr BinarySize FromTB(T value)
			{
				return FromGB(value * 1024);
			}

			template<class T>
			static constexpr BinarySize FromUnit(T value, BinarySizeUnit unit)
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
			template<class T>
			constexpr T GetAs(int64_t value) const
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
			constexpr BinarySize() = default;
			constexpr BinarySize(int64_t bytes)
				:m_Value(bytes)
			{
			}

		public:
			// Value testing
			constexpr bool IsNull() const
			{
				return m_Value == 0;
			}
			constexpr bool IsPositive() const
			{
				return m_Value > 0;
			}
			constexpr bool IsNegative() const
			{
				return m_Value < 0;
			}
			constexpr bool IsValid() const
			{
				return m_Value >= 0;
			}

			// Conversion
			template<class T = int64_t>
			constexpr T GetBytes() const
			{
				static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>, "invalid numeric type");

				return static_cast<T>(m_Value);
			}
			
			template<class T = int64_t>
			constexpr T GetKB() const
			{
				return GetAs<T>(m_Value);
			}
			
			template<class T = int64_t>
			constexpr T GetMB() const
			{
				return GetAs<T>(GetKB());
			}
			
			template<class T = int64_t>
			constexpr T GetGB() const
			{
				return GetAs<T>(GetMB());
			}
			
			template<class T = int64_t>
			constexpr T GetTB() const
			{
				return GetAs<T>(GetGB());
			}
			
			template<class T = int64_t>
			constexpr T GetAsUnit(BinarySizeUnit unit) const
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
			wxString Format(BinarySizeUnit unit = BinarySizeUnit::Auto, BinarySizeFormat format = BinarySizeFormat::Fractional|BinarySizeFormat::WithLabel, int precision = -1) const;

		public:
			// Comparison
			constexpr bool operator==(const BinarySize& other) const
			{
				return m_Value == other.m_Value;
			}
			constexpr bool operator!=(const BinarySize& other) const
			{
				return m_Value != other.m_Value;
			}
			constexpr bool operator<(const BinarySize& other) const
			{
				return m_Value < other.m_Value;
			}
			constexpr bool operator<=(const BinarySize& other) const
			{
				return m_Value <= other.m_Value;
			}
			constexpr bool operator>(const BinarySize& other) const
			{
				return m_Value >= other.m_Value;
			}
			constexpr bool operator>=(const BinarySize& other) const
			{
				return m_Value >= other.m_Value;
			}

			// Validity test (see  'IsValid')
			constexpr explicit operator bool() const
			{
				return IsValid();
			}
			constexpr bool operator!() const
			{
				return !IsValid();
			}
	};
}
