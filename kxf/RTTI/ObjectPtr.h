#pragma once
#include "Common.h"

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
}

namespace kxf
{
	template<class TValue_>
	class object_ptr final
	{
		static_assert(std::is_base_of_v<IObject, TValue_>, "RTTI object required");

		template<class T>
		friend class object_ptr;

		private:
			inline static RTTI::StdObjectDeleter<std::default_delete<IObject>> ms_DefaultDeleter;

		public:
			using TValue = TValue_;

		private:
			TValue* m_Value = nullptr;
			std::unique_ptr<RTTI::ObjectDeleter> m_Deleter;

		private:
			void AcquireRef(TValue* ptr, std::unique_ptr<RTTI::ObjectDeleter> deleter = {}) noexcept
			{
				m_Value = ptr;
				if (deleter.get() == &ms_DefaultDeleter)
				{
					deleter.release();
					m_Deleter.reset(&ms_DefaultDeleter);
				}
				else
				{
					m_Deleter = std::move(deleter);
				}
			}
			void ReleaseRef() noexcept
			{
				if (m_Deleter)
				{
					m_Deleter->Invoke(m_Value);
					if (m_Deleter.get() == &ms_DefaultDeleter)
					{
						m_Deleter.release();
					}
					else
					{
						m_Deleter.reset();
					}
				}
				m_Value = nullptr;
			}

			template<class T>
			void AssingUniquePtr(std::unique_ptr<T> ptr) noexcept
			{
				AcquireRef(ptr.release());
				m_Deleter.reset(&ms_DefaultDeleter);
			}

			template<class T, class TDeleter>
			void AssingUniquePtrWithDeleter(std::unique_ptr<T, TDeleter> ptr) noexcept
			{
				if constexpr(std::is_same_v<TDeleter, std::default_delete<T>>)
				{
					AcquireRef(ptr.release());
					m_Deleter.reset(&ms_DefaultDeleter);
				}
				else
				{
					AcquireRef(ptr.release(), std::make_unique<RTTI::StdObjectDeleter<TDeleter>>(ptr.get_deleter()));
				}
			}

		public:
			object_ptr() noexcept = default;
			object_ptr(std::nullptr_t) noexcept
			{
			}
			
			object_ptr(TValue* ptr) noexcept
			{
				AcquireRef(ptr);
			}
			object_ptr(TValue* ptr, std::unique_ptr<RTTI::ObjectDeleter> deleter) noexcept
			{
				AcquireRef(ptr, std::move(deleter));
			}
			
			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr(std::unique_ptr<T> ptr) noexcept
			{
				AssingUniquePtr(std::move(ptr));
			}

			template<class T, class TDeleter, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr(std::unique_ptr<TValue, TDeleter> ptr) noexcept
			{
				AssingUniquePtrWithDeleter(std::move(ptr));
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr(object_ptr<T>&& other) noexcept
			{
				AcquireRef(other.release(), std::move(other.m_Deleter));
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
				TValue* ptr = m_Value;
				m_Value = nullptr;
				ReleaseRef();

				return ptr;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool contains_same_value(const object_ptr<T>& other) const noexcept
			{
				return m_Value == other.get();
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			bool uses_same_deleter(const object_ptr<T>& other) const noexcept
			{
				return m_Deleter.get() == other.m_Deleter.get();
			}

			bool uses_default_deleter() const noexcept
			{
				return m_Deleter.get() == &ms_DefaultDeleter;
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
				AssingUniquePtr(std::move(other));
				return *this;
			}

			template<class T, class TDeleter, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr& operator=(std::unique_ptr<T, TDeleter> other) noexcept
			{
				AssingUniquePtrWithDeleter(std::move(other));
				return *this;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr& operator=(object_ptr<T>&& other) noexcept
			{
				ReleaseRef();
				AcquireRef(other.release(), std::move(other.m_Deleter));

				return *this;
			}

			template<class T, class = std::enable_if_t<std::is_base_of_v<TValue, T>>>
			object_ptr& operator=(const object_ptr<T>& other) noexcept
			{
				ReleaseRef();
				AcquireRef(other.get());

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
