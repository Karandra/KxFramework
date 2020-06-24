#pragma once
#include "../Common.h"

namespace kxf::COM::Private
{
	template<class T, class Traits>
	class BasicPtr
	{
		public:
			using TValue = T;

		protected:
			TValue* m_Value = nullptr;

		public:
			BasicPtr(TValue* ptr = nullptr) noexcept
				:m_Value(ptr)
			{
			}
			BasicPtr(BasicPtr&& other) noexcept
			{
				*this = std::move(other);
			}
			BasicPtr(const BasicPtr&) = delete;
			~BasicPtr() noexcept
			{
				Reset();
			}

		public:
			void Reset(TValue* ptr = nullptr) noexcept
			{
				Traits(m_Value).Reset(ptr);
			}
			TValue* Detach() noexcept
			{
				TValue* ptr = m_Value;
				m_Value = nullptr;
				return ptr;
			}

			TValue* Get() const noexcept
			{
				return m_Value;
			}
			void** GetAddress() const noexcept
			{
				return reinterpret_cast<void**>(const_cast<TValue**>(&m_Value));
			}
			
			operator const TValue*() const noexcept
			{
				return m_Value;
			}
			operator TValue*() noexcept
			{
				return m_Value;
			}

			const TValue& operator*() const noexcept
			{
				return *m_Value;
			}
			TValue& operator*() noexcept
			{
				return *m_Value;
			}
			TValue** operator&() noexcept
			{
				return &m_Value;
			}

			TValue* operator->() const noexcept
			{
				return m_Value;
			}
			TValue* operator->() noexcept
			{
				return m_Value;
			}

		public:
			explicit operator bool() const noexcept
			{
				return m_Value != nullptr;
			}
			bool operator!() const noexcept
			{
				return m_Value == nullptr;
			}

			bool operator==(const BasicPtr& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			bool operator==(const TValue* other) const noexcept
			{
				return m_Value == other;
			}
			bool operator==(const TValue& other) const noexcept
			{
				return m_Value == &other;
			}
			bool operator!=(const BasicPtr& other) const noexcept
			{
				return !(*this == other);
			}
			bool operator!=(const TValue* other) const noexcept
			{
				return !(*this == other);
			}
			bool operator!=(const TValue& other) const noexcept
			{
				return !(*this == other);
			}

			BasicPtr& operator=(BasicPtr&& other) noexcept
			{
				Reset(other.Detach());
				return *this;
			}
			BasicPtr& operator=(const BasicPtr&) = delete;
	};
}
