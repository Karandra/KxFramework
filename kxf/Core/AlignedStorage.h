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

			bool IsConstructed() const noexcept
			{
				return m_Value != nullptr;
			}
			void Destroy() noexcept(std::is_nothrow_destructible_v<TValue>)
			{
				if (m_Value)
				{
					m_Buffer.Destroy();
					m_Value = nullptr;
				}
			}

			void CopyFrom(const AlignedStorage& other) noexcept(std::is_nothrow_constructible_v<TValue, const TValue&>)
			{
				if (other.IsConstructed())
				{
					Destroy();
					Construct(*other.m_Value);
				}
				else if (IsConstructed())
				{
					Destroy();
				}
			}
			void MoveFrom(AlignedStorage&& other) noexcept(std::is_nothrow_constructible_v<TValue, TValue&&>)
			{
				if (other.IsConstructed())
				{
					Destroy();
					Construct(std::move(*other.m_Value));
				}
				else if (IsConstructed())
				{
					Destroy();
				}
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
