#pragma once
#include "Common.h"
#include "AlignedBuffer.h"

namespace kxf
{
	template<class TValue_, size_t t_Size = sizeof(TValue_), size_t t_Alignment = alignof(TValue_)>
	class AlignedStorage final
	{
		public:
			using TBuffer = AlignedBuffer<TValue_, t_Size, t_Alignment>;
			using TValue = typename TBuffer::TValue;

		private:
			TBuffer m_Buffer;
			TValue* m_Value = nullptr;

		public:
			AlignedStorage() noexcept = default;
			AlignedStorage(const AlignedStorage&) = delete;
			AlignedStorage(AlignedStorage&&) = delete;
			~AlignedStorage() noexcept = default;

		public:
			template<class... Args, std::enable_if_t<std::is_constructible_v<TValue, Args...>, int> = 0>
			TValue& Construct(Args&&... arg) noexcept(std::is_nothrow_constructible_v<TValue, Args...>)
			{
				if (!m_Value)
				{
					m_Value = &m_Buffer.Construct(std::forward<Args>(arg)...);
				}
				return *m_Value;
			}

			template<class T, std::enable_if_t<std::is_same_v<T, AlignedStorage> && std::is_copy_assignable_v<TValue>, int> = 0>
			void CopyFrom(const T& other) noexcept(std::is_nothrow_copy_assignable_v<TValue>)
			{
				if (other.IsConstructed())
				{
					if (IsConstructed())
					{
						*m_Value = *other.m_Value;
					}
					else
					{
						Construct(*other.m_Value);
					}
				}
				else if (IsConstructed())
				{
					Destroy();
				}
			}

			template<class T, std::enable_if_t<std::is_same_v<T, AlignedStorage> && std::is_move_assignable_v<TValue>, int> = 0>
			void MoveFrom(T&& other) noexcept(std::is_nothrow_move_assignable_v<TValue>)
			{
				if (other.IsConstructed())
				{
					if (IsConstructed())
					{
						*m_Value = std::move(*other.m_Value);
					}
					else
					{
						Construct(std::move(*other.m_Value));
					}
				}
				else if (IsConstructed())
				{
					Destroy();
				}
			}

			template<std::enable_if_t<std::is_move_constructible_v<TValue>, int> = 0>
			TValue TakeValue() noexcept
			{
				if (m_Value)
				{
					return std::move(*m_Value);
				}
				return {};
			}

			void Destroy() noexcept(std::is_nothrow_destructible_v<TValue>)
			{
				if (m_Value)
				{
					m_Buffer.Destroy();
					m_Value = nullptr;
				}
			}

			bool IsConstructed() const noexcept
			{
				return m_Value != nullptr;
			}

		public:
			const TBuffer& GetBuffer() const
			{
				return m_Buffer;
			}
			TBuffer& GetBuffer()
			{
				return m_Buffer;
			}

			size_t size() const noexcept
			{
				return m_Buffer.size();
			}
			size_t alignment() const noexcept
			{
				return m_Buffer.alignment();
			}

			const TValue* get() const noexcept
			{
				return m_Value;
			}
			TValue* get() noexcept
			{
				return m_Value;
			}

		public:
			const TValue* operator&() const noexcept
			{
				return get();
			}
			TValue* operator&() noexcept
			{
				return get();
			}

			const TValue& operator*() const noexcept
			{
				return *get();
			}
			TValue& operator*() noexcept
			{
				return *get();
			}

			const TValue* operator->() const noexcept
			{
				return get();
			}
			TValue* operator->() noexcept
			{
				return get();
			}

			AlignedStorage& operator=(const AlignedStorage&) = delete;
			AlignedStorage& operator=(AlignedStorage&&) = delete;
	};
}
