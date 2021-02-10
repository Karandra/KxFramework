#pragma once
#include "kxf/Common.hpp"

namespace kxf
{
	template<class T, class TFinalizer, T null2 = static_cast<T>(0)>
	class handle_ptr final
	{
		static_assert(std::is_integral_v<T> || std::is_pointer_v<T>);

		private:
			static inline constexpr T null = static_cast<T>(0);

		private:
			T m_Handle = null;
			TFinalizer m_Finalizer = nullptr;

		private:
			void Finalize(T handle = null) noexcept
			{
				if (m_Finalizer && !is_null())
				{
					std::invoke(get_deleter(), m_Handle);
				}
				m_Handle = handle;
			}
			T Detach() noexcept
			{
				auto handle = m_Handle;
				m_Handle = null;

				return handle;
			}

		public:
			handle_ptr() noexcept = default;
			handle_ptr(T handle, TFinalizer finalizer = nullptr) noexcept
				:m_Handle(handle), m_Finalizer(finalizer)
			{
			}

			handle_ptr(const handle_ptr&) = delete;
			handle_ptr(handle_ptr&& other) noexcept
				:m_Handle(other.Detach()), m_Finalizer(other.m_Finalizer)
			{
			}
			~handle_ptr()
			{
				Finalize();
			}

		public:
			bool is_null() const noexcept
			{
				return m_Handle == null || m_Handle == null2;
			}

			T get() const noexcept
			{
				return m_Handle;
			}
			TFinalizer get_deleter() const noexcept
			{
				return {};
			}

			void reset(T handle = null) noexcept
			{
				Finalize(handle);
			}
			T release() noexcept
			{
				return Detach();
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

			T operator*() const noexcept
			{
				return m_Handle;
			}

			handle_ptr& operator=(const handle_ptr&) = delete;
			handle_ptr& operator=(handle_ptr&& other) noexcept
			{
				m_Handle = other.Detach();
				m_Finalizer = other.m_Finalizer;

				return *this;
			}
	};
}

namespace kxf
{
	template<class T, T null2, class TFinalizer>
	handle_ptr<T, TFinalizer, null2> make_handle_ptr(T handle, TFinalizer finalizer) noexcept
	{
		return {handle, finalizer};
	}

	template<class T, class TFinalizer>
	handle_ptr<T, TFinalizer> make_handle_ptr(T handle, TFinalizer finalizer) noexcept
	{
		return {handle, finalizer};
	}
}
