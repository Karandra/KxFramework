#pragma once
#include "Common.h"

namespace kxf
{
	template<class T, size_t t_Size, size_t t_Alignment>
	class LocalPImpl final
	{
		public:
			using TValue = T;

		private:
			alignas(t_Alignment) uint8_t m_Buffer[t_Size] = {};
			T* m_Value = nullptr;

		public:
			constexpr LocalPImpl() noexcept = default;
			LocalPImpl(const LocalPImpl&) = delete;
			LocalPImpl(LocalPImpl&&) = delete;

		public:
			template<class... Args>
			constexpr T& Construct(Args&&... arg) noexcept(std::is_nothrow_constructible_v<TValue>)
			{
				static_assert(t_Size >= sizeof(TValue), "insufficient buffer size");
				static_assert(t_Alignment == alignof(TValue), "alignment doesn't match");

				if (!m_Value)
				{
					auto DoConstruct = [&]()
					{
						new (data()) TValue(std::forward<Args>(arg)...);
						m_Value = reinterpret_cast<TValue*>(m_Buffer);
					};

					if constexpr(std::is_nothrow_constructible_v<TValue>)
					{
						DoConstruct();
					}
					else
					{
						try
						{
							DoConstruct();
						}
						catch (...)
						{
							Destruct();
							throw;
						}
					}
				}
				return *m_Value;
			}
			
			constexpr void Destruct() noexcept(std::is_nothrow_destructible_v<TValue>)
			{
				if (m_Value)
				{
					m_Value->~TValue();
					m_Value = nullptr;
				}
			}
			void Zero() noexcept
			{
				if (m_Value)
				{
					std::memset(m_Buffer, 0, t_Size);
					m_Value = nullptr;
				}
			}

			constexpr bool IsConstructed() const noexcept
			{
				return m_Value != nullptr;
			}

		public:
			constexpr const void* data() const noexcept
			{
				return reinterpret_cast<const void*>(m_Buffer);
			}
			constexpr void* data() noexcept
			{
				return reinterpret_cast<void*>(m_Buffer);
			}
			
			constexpr size_t size() const noexcept
			{
				return t_Size;
			}
			constexpr size_t effective_size() const noexcept
			{
				return sizeof(*this);
			}
			constexpr size_t alignment() const noexcept
			{
				return t_Alignment;
			}

			constexpr const T* get() const noexcept
			{
				return m_Value;
			}
			constexpr T* get() noexcept
			{
				return m_Value;
			}

		public:
			constexpr const T* operator&() const noexcept
			{
				return get();
			}
			constexpr T* operator&() noexcept
			{
				return get();
			}

			constexpr const T& operator*() const noexcept
			{
				return *get();
			}
			constexpr T& operator*() noexcept
			{
				return *get();
			}

			constexpr const T* operator->() const noexcept
			{
				return get();
			}
			constexpr T* operator->() noexcept
			{
				return get();
			}

			LocalPImpl& operator=(const LocalPImpl&) = delete;
			LocalPImpl& operator=(LocalPImpl&&) = delete;
	};

	template<class T>
	using LocalPImplComplete = LocalPImpl<T, sizeof(T), alignof(T)>;
}
