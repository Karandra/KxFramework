#pragma once
#include "Common.h"

namespace kxf
{
	template<class TValue_>
	class object_ptr final
	{
		public:
			using TValue = TValue_;

		private:
			TValue* m_Value = nullptr;

		private:
			void ReleaseRef() noexcept
			{
				if (m_Value)
				{
					static_cast<IObject&>(*m_Value).DoReleaseRef();
					m_Value = nullptr;
				}
			}
			void AcquireRef(TValue* ptr) noexcept
			{
				m_Value = ptr;
				if (ptr)
				{
					static_cast<IObject&>(*ptr).DoAddRef();
				}
			}

		public:
			object_ptr() noexcept = default;
			object_ptr(TValue* ptr) noexcept
			{
				AcquireRef(ptr);
			}
			object_ptr(std::nullptr_t) noexcept
			{
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr(object_ptr<T>&& other) noexcept
				:m_Value(other.release())
			{
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr(const object_ptr<T>& other) noexcept
			{
				AcquireRef(other.get());
			}

			~object_ptr() noexcept
			{
				ReleaseRef();
			}

		public:
			bool is_null() const noexcept
			{
				return m_Value == nullptr;
			}
			TValue* get() const noexcept
			{
				return m_Value;
			}
			void** get_address() noexcept
			{
				return reinterpret_cast<void**>(&m_Value);
			}

			void reset(TValue* ptr = nullptr) noexcept
			{
				ReleaseRef();
				AcquireRef(ptr);
			}
			TValue* release() noexcept
			{
				TValue* ptr = m_Value;
				m_Value = nullptr;
				return ptr;
			}

		public:
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

			const TValue* operator->() const noexcept
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
				return !is_null();
			}
			bool operator!() const noexcept
			{
				return is_null();
			}

			object_ptr& operator=(std::nullptr_t) noexcept
			{
				ReleaseRef();
				return *this;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr& operator=(object_ptr<T>&& other) noexcept
			{
				ReleaseRef();
				m_Value = other.release();

				return *this;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr& operator=(const object_ptr<T>& other) noexcept
			{
				ReleaseRef();
				AcquireRef(other.get());

				return *this;
			}

			bool operator==(std::nullptr_t) const noexcept
			{
				return m_Value == nullptr;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool operator==(const T* other) const noexcept
			{
				return m_Value == other;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool operator==(const T& other) const noexcept
			{
				return m_Value == &other;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool operator==(const object_ptr<T>& other) const noexcept
			{
				return m_Value == other.get();
			}

			bool operator!=(std::nullptr_t) const noexcept
			{
				return m_Value != nullptr;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool operator!=(const T* other) const noexcept
			{
				return m_Value != other;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool operator!=(const T& other) const noexcept
			{
				return m_Value != &other;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool operator!=(const object_ptr<T>& other) const noexcept
			{
				return m_Value != other.get();
			}
	};
}
