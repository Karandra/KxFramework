#pragma once
#include "Common.h"

namespace kxf::Private
{
	template<class R, class... Args>
	class unique_function_target_interface
	{
		public:
			virtual ~unique_function_target_interface() = default;

		public:
			virtual R invoke(Args&&... arg) const = 0;
			virtual const std::type_info& target_type() const = 0;
	};

	template<class F, class R, class... Args>
	class unique_function_target_impl final: public unique_function_target_interface<R, Args...>
	{
		private:
			mutable F m_Func;

		public:
			template<class TFunc>
			unique_function_target_impl(TFunc&& func)
				:m_Func(std::forward<TFunc>(func))
			{
			}

		public:
			R invoke(Args&&... arg) const override
			{
				return std::invoke(m_Func, std::forward<Args>(arg)...);
			}
			const std::type_info& target_type() const override
			{
				return typeid(F);
			}
	};
}

namespace kxf
{
	// Credit: https://riptutorial.com/cplusplus/example/18042/a-move-only--std--function-
	template<class TSignature>
	class unique_function;

	template<class R, class... Args>
	class unique_function<R(Args...)>
	{
		public:
			using result_type = R;

		private:
			template<class T>
			using impl_t = Private::unique_function_target_impl<T, R, Args...>;

			template<class TFunc>
			using is_unique_function = std::is_same<std::decay_t<TFunc>, unique_function>;

		private:
			std::unique_ptr<Private::unique_function_target_interface<R, Args...>> m_Target;

		private:
			template<class TFunc>
			static constexpr bool CheckTargetType() noexcept
			{
				return !is_unique_function<TFunc>::value && std::is_invocable_r_v<R, TFunc, Args...>;
			}

			template<class TFunc>
			static auto CreateTarget(TFunc&& func)
			{
				using Fx = std::decay_t<TFunc>;
				using Ix = impl_t<Fx>;

				return std::make_unique<Ix>(std::forward<TFunc>(func));
			}

			template<class T>
			T* GetTargetImpl() const noexcept
			{
				return dynamic_cast<impl_t<T>*>(m_Target.get());
			}

		public:
			unique_function() = default;
			unique_function(unique_function&&) noexcept = default;
			unique_function(const unique_function&) = delete;

			template<class TFunc, std::enable_if_t<CheckTargetType<TFunc>(), int> = 0>
			unique_function(TFunc&& func)
				:m_Target(CreateTarget(std::forward<TFunc>(func)))
			{
			}

		public:
			bool is_null() const noexcept
			{
				return m_Target == nullptr;
			}

			void swap(unique_function& other) noexcept
			{
				std::swap(m_Target, other.m_Target);
			}

			R invoke(Args&&... arg) const
			{
				return m_Target->invoke(std::forward<Args>(arg)...);
			}

			template<class TFunc, std::enable_if_t<CheckTargetType<TFunc>(), int> = 0>
			void assign(TFunc&& func)
			{
				m_Target = CreateTarget(std::forward<TFunc>(func));
			}

			template<class T>
			T* target()
			{
				return GetTargetImpl<T>();
			}

			template<class T>
			const T* target() const
			{
				return GetTargetImpl<T>();
			}

			const std::type_info& target_type() const
			{
				if (!is_null())
				{
					return m_Target->target_type();
				}
				return typeid(void);
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

			R operator()(Args&&... arg) const
			{
				return invoke(std::forward<Args>(arg)...);
			}

			unique_function& operator=(unique_function&&) noexcept = default;
			unique_function& operator=(const unique_function&) = delete;

			template<class TFunc, std::enable_if_t<CheckTargetType<TFunc>(), int> = 0>
			unique_function& operator=(TFunc&& func)
			{
				assign(std::forward<TFunc>(func));
				return *this;
			}
	};
}
