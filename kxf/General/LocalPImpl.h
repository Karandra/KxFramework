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
			TValue* m_Value = nullptr;

		public:
			LocalPImpl() noexcept(noexcept(Construct()))
			{
				Construct();
			}

			template<class... Args>
			LocalPImpl(Args&&... arg) noexcept(noexcept(Construct(Args...)))
			{
				Construct(std::forward<Args>(arg)...);
			}

			LocalPImpl(const LocalPImpl& other) noexcept(std::is_nothrow_copy_constructible_v<TValue>)
			{
				Construct(*other.m_Value);
			}
			LocalPImpl(LocalPImpl&& other) noexcept(std::is_nothrow_move_constructible_v<TValue>)
			{
				Construct(std::move(*other.m_Value));
			}
			~LocalPImpl() noexcept(noexcept(Destruct()))
			{
				Destruct();
			}

		public:
			template<class... Args>
			T& Construct(Args&&... arg) noexcept(std::is_nothrow_constructible_v<TValue>)
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
			
			void Destruct() noexcept(std::is_nothrow_destructible_v<TValue>)
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

			bool IsConstructed() const noexcept
			{
				return m_Value != nullptr;
			}

		public:
			const void* data() const noexcept
			{
				return reinterpret_cast<const void*>(m_Buffer);
			}
			void* data() noexcept
			{
				return reinterpret_cast<void*>(m_Buffer);
			}
			
			size_t size() const noexcept
			{
				return t_Size;
			}
			size_t alignment() const noexcept
			{
				return t_Alignment;
			}

			const T* get() const noexcept
			{
				return m_Value;
			}
			T* get() noexcept
			{
				return m_Value;
			}

		public:
			const T* operator&() const noexcept
			{
				return get();
			}
			T* operator&() noexcept
			{
				return get();
			}

			const T& operator*() const noexcept
			{
				return *get();
			}
			T& operator*() noexcept
			{
				return *get();
			}

			const T* operator->() const noexcept
			{
				return get();
			}
			T* operator->() noexcept
			{
				return get();
			}

			LocalPImpl& operator=(const LocalPImpl& other) noexcept(std::is_nothrow_copy_assignable_v<TValue>)
			{
				*m_Value = *other.m_Value;
				return *this;
			}
			LocalPImpl& operator=(LocalPImpl&& other) noexcept(std::is_nothrow_move_assignable_v<TValue>)
			{
				*m_Value = std::move(*other.m_Value);
				return *this;
			}
	};

	template<class T>
	using LocalPImplComplete = LocalPImpl<T, sizeof(T), alignof(T)>;
}
