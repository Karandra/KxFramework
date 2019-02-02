#pragma once
#include "Common.h"
#include <KxFramework/KxWithOptions.h>

namespace KxDataView2
{
	class ColumnID
	{
		public:
			using TValue = int32_t;

		private:
			constexpr static TValue GetInvalidValue() noexcept
			{
				return std::numeric_limits<TValue>::min();
			}
			
			template<class T> constexpr static bool CheckType()
			{
				return std::is_integral_v<T> || std::is_enum_v<T>;
			}
			template<class T> static void AssertType()
			{
				static_assert(CheckType<T>(), "only integer types are allowed");
			}

		private:
			TValue m_Value;

		public:
			ColumnID(TValue value = GetInvalidValue()) noexcept
				:m_Value(value)
			{
			}
			ColumnID(const ColumnID& other) noexcept
			{
				*this = other;
			}
			ColumnID(ColumnID&& other) noexcept
			{
				*this = std::move(other);
			}
			
			template<class T, typename std::enable_if<CheckType<T>() && !std::is_same_v<std::decay_t<T>, ColumnID>, int>::type = 0>
			ColumnID(const T& value) noexcept
				:m_Value(static_cast<TValue>(value))
			{
			}

		public:
			bool IsOK() const noexcept
			{
				return m_Value != GetInvalidValue();
			}
			void MakeNull() noexcept
			{
				m_Value = GetInvalidValue();
			}
			
			explicit operator bool() const noexcept
			{
				return IsOK();
			}
			bool operator!() const noexcept
			{
				return !IsOK();
			}
			
			TValue GetValue() const noexcept
			{
				return m_Value;
			}
			operator TValue() const noexcept
			{
				return GetValue();
			}
			
			template<class T> T GetValue() const noexcept
			{
				AssertType<T>();
				return static_cast<T>(m_Value);
			}
			template<class T> operator T() const noexcept
			{
				return GetValue<T>();
			}

		public:
			ColumnID& operator=(const ColumnID& other) noexcept
			{
				m_Value = other.m_Value;
				return *this;
			}
			ColumnID& operator=(ColumnID&& other) noexcept
			{
				m_Value = other.m_Value;
				other.MakeNull();

				return *this;
			}
			
			bool operator==(const TValue other) const noexcept
			{
				return m_Value == other;
			}
			bool operator!=(const TValue other) const noexcept
			{
				return !(*this == other);
			}
			
			template<class T> bool operator==(const T& other) const noexcept
			{
				AssertType<T>();
				return m_Value == static_cast<TValue>(other);
			}
			template<class T> bool operator!=(const T& other) const noexcept
			{
				return !(*this == other);
			}
	};
}
