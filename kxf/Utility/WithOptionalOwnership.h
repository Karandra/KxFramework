#pragma once
#include "Common.h"

namespace kxf::Utility
{
	template<class T>
	class WithOptionalOwnership final
	{
		public:
			using TValue = T;

		private:
			TValue* m_Value = nullptr;
			bool m_Owned = false;

		private:
			void Destroy() noexcept
			{
				if (m_Owned)
				{
					delete m_Value;
				}

				m_Owned = false;
				m_Value = nullptr;
			}

		public:
			WithOptionalOwnership() noexcept = default;
			WithOptionalOwnership(TValue& value) noexcept
			{
				Assign(value);
			}
			WithOptionalOwnership(std::unique_ptr<TValue> value) noexcept
			{
				Assign(std::move(value));
			}
			WithOptionalOwnership(const WithOptionalOwnership&) = delete;
			WithOptionalOwnership(WithOptionalOwnership&& other) noexcept
			{
				*this = std::move(other);
			}
			~WithOptionalOwnership() noexcept
			{
				Destroy();
			}

		public:
			void Assign(TValue& value) noexcept
			{
				Destroy();
				m_Value = &value;
				m_Owned = false;
			}
			void Assign(std::unique_ptr<TValue> value) noexcept
			{
				Destroy();
				m_Value = value.release();
				m_Owned = true;
			}

			bool IsOwned() const noexcept
			{
				return m_Owned;
			}
			bool IsSame(const WithOptionalOwnership& other) const noexcept
			{
				return m_Value == other.m_Value;
			}
			
			TValue* Get() const&
			{
				return m_Value;
			}
			std::unique_ptr<TValue> GetUnique() &&
			{
				if (m_Owned)
				{
					m_Owned = false;
					return std::unique_ptr<TValue>(ExchangeResetAndReturn(m_Value, nullptr));
				}
				return nullptr;
			}

			TValue* operator->() const noexcept
			{
				return m_Value;
			}
			operator TValue*() const noexcept
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

		public:
			WithOptionalOwnership& operator=(const WithOptionalOwnership&) = delete;
			WithOptionalOwnership& operator=(WithOptionalOwnership&& other) noexcept
			{
				Destroy();
				m_Value = ExchangeResetAndReturn(other.m_Value, nullptr);
				m_Owned = ExchangeResetAndReturn(other.m_Owned, false);

				return *this;
			}

			explicit operator bool() const noexcept
			{
				return m_Value != nullptr;
			}
			bool operator!() const noexcept
			{
				return m_Value == nullptr;
			}

	};
}
