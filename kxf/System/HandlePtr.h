#pragma once
#include "kxf/Common.hpp"

namespace kxf::Private
{
	template<class TValue_, class TFinalizer_, TFinalizer_ finalizer_ = nullptr, TValue_ null2_ = static_cast<TValue_>(0)>
	class basic_handle_ptr final
	{
		static_assert(std::is_integral_v<TValue_> || std::is_pointer_v<TValue_>);

		public:
			using TValue = TValue_;
			using TFinalizer = TFinalizer_;

		public:
			static inline constexpr TValue null = static_cast<TValue>(0);
			static inline constexpr TValue null2 = null2_;

		private:
			TValue m_Handle = null;
			TFinalizer m_Finalizer = finalizer_;

		private:
			void Finalize(TValue handle = null) noexcept
			{
				if (m_Finalizer && !is_null())
				{
					std::invoke(get_deleter(), m_Handle);
				}
				m_Handle = handle;
			}
			TValue Detach() noexcept
			{
				auto handle = m_Handle;
				m_Handle = null;

				return handle;
			}

		public:
			basic_handle_ptr() noexcept = default;
			basic_handle_ptr(TValue handle, TFinalizer finalizer = finalizer_) noexcept
				:m_Handle(handle), m_Finalizer(finalizer_ ? finalizer_ : finalizer)
			{
			}

			basic_handle_ptr(const basic_handle_ptr&) = delete;
			basic_handle_ptr(basic_handle_ptr&& other) noexcept
				:m_Handle(other.Detach()), m_Finalizer(finalizer_ ? finalizer_ : other.m_Finalizer)
			{
			}
			~basic_handle_ptr()
			{
				Finalize();
			}

		public:
			bool is_null() const noexcept
			{
				return m_Handle == null || m_Handle == null2;
			}

			TValue get() const noexcept
			{
				return m_Handle;
			}
			TFinalizer get_deleter() const noexcept
			{
				return m_Finalizer;
			}

			void reset(TValue handle = null) noexcept
			{
				Finalize(handle);
			}
			TValue release() noexcept
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

			TValue operator*() const noexcept
			{
				return m_Handle;
			}

			basic_handle_ptr& operator=(const basic_handle_ptr&) = delete;
			basic_handle_ptr& operator=(basic_handle_ptr&& other) noexcept
			{
				m_Handle = other.Detach();
				m_Finalizer = finalizer_ ? finalizer_ : other.m_Finalizer;

				return *this;
			}
	};
}

namespace kxf
{
	template<class TValue, class TFinalizer, TValue null2_ = static_cast<TValue>(0)>
	using handle_ptr = Private::basic_handle_ptr<TValue, TFinalizer, nullptr, null2_>;

	template<class TValue, auto finalizer, TValue null2_ = static_cast<TValue>(0)>
	using bound_handle_ptr = Private::basic_handle_ptr<TValue, decltype(finalizer), finalizer, null2_>;
}

namespace kxf
{
	template<class TValue, class TFinalizer, TValue null2 = static_cast<TValue>(0)>
	handle_ptr<TValue, TFinalizer, null2> make_handle_ptr(TValue handle, TFinalizer finalizer) noexcept
	{
		return {handle, finalizer};
	}

	template<auto finalizer, class TValue, TValue null2 = static_cast<TValue>(0)>
	bound_handle_ptr<TValue, finalizer, null2> make_handle_ptr(TValue handle) noexcept
	{
		return handle;
	}
}
