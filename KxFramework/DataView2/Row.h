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
			constexpr static TValue GetInvalidValue()
			{
				return std::numeric_limits<TValue>::max();
			}

		private:
			TValue m_Value;

		public:
			Row(TValue value = GetInvalidValue())
				:m_Value(value)
			{
			}

		public:
			bool IsOK() const
			{
				return m_Value != GetInvalidValue();
			}
			void MakeInvalid()
			{
				m_Value = GetInvalidValue();
			}
			
			explicit operator bool() const
			{
				return IsOK();
			}
			bool operator!() const
			{
				return !IsOK();
			}
	
			TValue GetValue() const
			{
				return m_Value;
			}
			operator TValue() const
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
			
			Row& operator++()
			{
				++m_Value;
				return *this;
			}
			Row& operator--()
			{
				--m_Value;
				return *this;
			}
			Row operator++(int) const
			{
				return Row(m_Value + 1);
			}
			Row operator--(int) const
			{
				return Row(m_Value - 1);
			}
	};
}
