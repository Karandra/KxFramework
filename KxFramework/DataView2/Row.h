#pragma once
#include "Common.h"
#include <KxFramework/KxWithOptions.h>

namespace Kx::DataView2
{
	class Row
	{
		public:
			using TValue = size_t;
			using Vector = std::vector<TValue>;

		private:
			constexpr static TValue GetInvalidValue() noexcept
			{
				return std::numeric_limits<TValue>::max();
			}

		private:
			TValue m_Value;

		public:
			Row(TValue value = GetInvalidValue()) noexcept
				:m_Value(value)
			{
			}

		public:
			bool IsOK() const noexcept
			{
				return m_Value != GetInvalidValue();
			}
			void MakeInvalid() noexcept
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

		public:
			template<class T> bool operator==(const T& other) const
			{
				return m_Value == other;
			}
			template<class T> bool operator!=(const T& other) const
			{
				return m_Value != other;
			}

			template<class T> bool operator<(const T& other) const
			{
				return m_Value < other;
			}
			template<class T> bool operator<=(const T& other) const
			{
				return m_Value <= other;
			}
			template<class T> bool operator>(const T& other) const
			{
				return m_Value > other;
			}
			template<class T> bool operator>=(const T& other) const
			{
				return m_Value >= other;
			}
			
			template<class T> Row& operator+=(const T& other)
			{
				m_Value += other;
				return *this;
			}
			template<class T> Row& operator-=(const T& other)
			{
				m_Value -= other;
				return *this;
			}

			Row& operator++() noexcept
			{
				++m_Value;
				return *this;
			}
			Row& operator--() noexcept
			{
				--m_Value;
				return *this;
			}
			Row operator++(int) const noexcept
			{
				return Row(m_Value + 1);
			}
			Row operator--(int) const noexcept
			{
				return Row(m_Value - 1);
			}
	};
}
