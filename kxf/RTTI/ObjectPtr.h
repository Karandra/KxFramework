#pragma once
#include "Common.h"
#include <memory>

namespace kxf
{
	class IObject;
}

namespace kxf::RTTI
{
	class ObjectDeleter
	{
		public:
			virtual ~ObjectDeleter() = default;

		public:
			virtual void Invoke(IObject* object) noexcept = 0;
	};

	template<class T>
	class StdObjectDeleter final: public ObjectDeleter
	{
		private:
			T m_Deleter;

		public:
			StdObjectDeleter() noexcept = default;
			StdObjectDeleter(const T& deleter)
				:m_Deleter(deleter)
			{
			}

		public:
			void Invoke(IObject* object) noexcept override
			{
				m_Deleter(object);
			}
	};

	KX_API ObjectDeleter& GetDefaultDeleter() noexcept;
}

namespace kxf
{
	template<class TValue_>
	class object_ptr final
	{
		template<class T>
		friend class object_ptr;

		public:
			using TValue = TValue_;

		private:
			TValue* m_Value = nullptr;
			std::unique_ptr<RTTI::ObjectDeleter> m_Deleter;

		private:
			void AcquireRef(TValue* ptr, std::unique_ptr<RTTI::ObjectDeleter> deleter = {}) noexcept
			{
				m_Value = ptr;
				m_Deleter = std::move(deleter);
			}
			void ReleaseRef() noexcept
			{
				if (m_Deleter)
				{
					if constexpr(std::is_const_v<TValue>)
					{
						using NC = std::remove_const_t<TValue>;
						m_Deleter->Invoke(const_cast<NC*>(m_Value));
					}
					else
					{
						m_Deleter->Invoke(m_Value);
					}
					
					RTTI::ObjectDeleter& defaultDeleter = RTTI::GetDefaultDeleter();
					if (m_Deleter.get() == &defaultDeleter)
					{
						static_cast<void>(m_Deleter.release());
					}
					else
					{
						m_Deleter.reset();
					}
				}
				m_Value = nullptr;
			}

			template<class T>
			void AcquireUniquePtr(std::unique_ptr<T> ptr) noexcept
			{
				AcquireRef(ptr.release());
				m_Deleter.reset(&RTTI::GetDefaultDeleter());
			}

			template<class T, class TDeleter>
			void AcquireUniquePtrWithDeleter(std::unique_ptr<T, TDeleter> ptr) noexcept
			{
				if constexpr(std::is_same_v<TDeleter, std::default_delete<T>>)
				{
					AcquireRef(ptr.release());
					m_Deleter.reset(&RTTI::GetDefaultDeleter());
				}
				else
				{
					AcquireRef(ptr.release(), std::make_unique<RTTI::StdObjectDeleter<TDeleter>>(ptr.get_deleter()));
				}
			}

			TValue* ExchangeValue(TValue* newPtr = nullptr) noexcept
			{
				TValue* oldPtr = m_Value;
				m_Value = newPtr;
				return oldPtr;
			}

		public:
			object_ptr() noexcept = default;
			object_ptr(std::nullptr_t) noexcept
			{
			}
			
			explicit object_ptr(TValue* ptr) noexcept
			{
				AcquireRef(ptr);
			}
			explicit object_ptr(TValue* ptr, std::unique_ptr<RTTI::ObjectDeleter> deleter) noexcept
			{
				AcquireRef(ptr, std::move(deleter));
			}
			
			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr(std::unique_ptr<T> ptr) noexcept
			{
				AcquireUniquePtr(std::move(ptr));
			}

			template<class T, class TDeleter, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr(std::unique_ptr<TValue, TDeleter> ptr) noexcept
			{
				AcquireUniquePtrWithDeleter(std::move(ptr));
			}

			object_ptr(object_ptr&& other) noexcept
			{
				AcquireRef(other.ExchangeValue(), std::move(other.m_Deleter));
			}
			object_ptr(const object_ptr& other) noexcept
			{
				AcquireRef(other.m_Value);
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr(object_ptr<T>&& other) noexcept
			{
				AcquireRef(other.ExchangeValue(), std::move(other.m_Deleter));
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr(const object_ptr<T>& other) noexcept
			{
				AcquireRef(other.m_Value);
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
			bool is_reference() const noexcept
			{
				return m_Deleter == nullptr;
			}
			
			TValue* get() noexcept
			{
				return m_Value;
			}
			const TValue* get() const noexcept
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
				TValue* ptr = ExchangeValue();
				ReleaseRef();

				return ptr;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool contains_same_value(const object_ptr<T>& other) const noexcept
			{
				return m_Value == other.get();
			}

		public:
			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool uses_same_deleter(const object_ptr<T>& other) const noexcept
			{
				return m_Deleter.get() == other.m_Deleter.get();
			}

			bool uses_default_deleter() const noexcept
			{
				return m_Deleter.get() == &RTTI::GetDefaultDeleter();
			}

			std::unique_ptr<RTTI::ObjectDeleter> get_deleter() &&
			{
				return std::move(m_Deleter);
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

			// Assignment
			object_ptr& operator=(std::nullptr_t) noexcept
			{
				ReleaseRef();
				return *this;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr& operator=(std::unique_ptr<T> other) noexcept
			{
				AcquireUniquePtr(std::move(other));
				return *this;
			}

			template<class T, class TDeleter, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr& operator=(std::unique_ptr<T, TDeleter> other) noexcept
			{
				AcquireUniquePtrWithDeleter(std::move(other));
				return *this;
			}

			object_ptr& operator=(object_ptr&& other) noexcept
			{
				ReleaseRef();
				AcquireRef(other.ExchangeValue(), std::move(other.m_Deleter));

				return *this;
			}
			object_ptr& operator=(const object_ptr& other) noexcept
			{
				ReleaseRef();
				AcquireRef(other.m_Value);

				return *this;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr& operator=(object_ptr<T>&& other) noexcept
			{
				ReleaseRef();
				AcquireRef(other.ExchangeValue(), std::move(other.m_Deleter));

				return *this;
			}
			
			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr& operator=(const object_ptr<T>& other) noexcept
			{
				ReleaseRef();
				AcquireRef(other.m_Value);

				return *this;
			}

			// Comparison
			bool operator==(std::nullptr_t) const noexcept
			{
				return is_null();
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool operator==(const T* other) const noexcept
			{
				return m_Value == other;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool operator==(const object_ptr<T>& other) const noexcept
			{
				return contains_same_value(other) && uses_same_deleter(other);
			}

			bool operator!=(std::nullptr_t) const noexcept
			{
				return !is_null();
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool operator!=(const T* other) const noexcept
			{
				return m_Value != other;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool operator!=(const object_ptr<T>& other) const noexcept
			{
				return !contains_same_value(other) || !uses_same_deleter(other);
			}
	};
}

namespace kxf::RTTI
{
	template<class T>
	object_ptr<T> assume_non_owned(T& value) noexcept
	{
		return object_ptr<T>(&value);
	}

	template<class T1, class T2>
	object_ptr<T2> cast_object_ptr(object_ptr<T1> ptr) noexcept
	{
		auto deleter = std::move(ptr).get_deleter();
		T2* object = static_cast<T2*>(ptr.release());
		return object_ptr<T2>(object, std::move(deleter));
	}
}
