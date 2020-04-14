#pragma once
#include "Common.h"

namespace KxDataView2
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
			TValue m_Value = GetInvalidValue();

		public:
			Row(TValue value = GetInvalidValue()) noexcept
				:m_Value(value)
			{
			}
			Row(const Row& other)
				:m_Value(other.m_Value)
			{
			}
			Row(Row&& other)
				:m_Value(other.m_Value)
			{
				other.MakeNull();
			}

			Row& operator=(const Row& other)
			{
				m_Value = other.m_Value;
				return *this;
			}
			Row& operator=(Row&& other)
			{
				m_Value = other.m_Value;
				other.MakeNull();
				return *this;
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
			Row operator++(int) noexcept
			{
				size_t oldValue = m_Value + 1;
				++m_Value;
				return oldValue;
			}
			Row operator--(int) noexcept
			{
				size_t oldValue = m_Value - 1;
				--m_Value;
				return oldValue;
			}
	};
}
