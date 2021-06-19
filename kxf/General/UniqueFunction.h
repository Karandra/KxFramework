#pragma once
#include "Common.h"
#include "kxf/Utility/Memory.h"

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

			virtual void move_storage(void* address, unique_function_target_interface&& other) noexcept = 0;
			virtual void destroy_storage() noexcept = 0;

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

			void move_storage(void* address, unique_function_target_interface<R, Args...>&& other) noexcept override
			{
				Utility::ConstructAt<unique_function_target_impl>(address, std::move(static_cast<unique_function_target_impl&>(other)));
			}
			void destroy_storage() noexcept override
			{
				std::destroy_at(this);
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
			using interface_t = Private::unique_function_target_interface<R, Args...>;

			template<class TFunc>
			using impl_t = Private::unique_function_target_impl<TFunc, R, Args...>;

			template<class TFunc>
			using is_unique_function = std::is_same<std::decay_t<TFunc>, unique_function>;

		private:
			std::array<uint8_t, sizeof(void*) * 6> m_LocalBuffer;
			std::unique_ptr<interface_t> m_HeapBuffer;
			interface_t* m_Target = nullptr;

		private:
			template<class TFunc>
			static constexpr bool CheckTargetType() noexcept
			{
				return !is_unique_function<TFunc>::value && std::is_invocable_r_v<R, TFunc, Args...>;
			}

			template<class TFunc>
			interface_t* CreateTarget(TFunc&& func)
			{
				using Fx = std::decay_t<TFunc>;
				using Ix = impl_t<Fx>;

				void* address = m_LocalBuffer.data();
				size_t size = m_LocalBuffer.size();
				if (std::align(alignof(Ix), sizeof(Ix), address, size))
				{
					m_Target = Utility::ConstructAt<Ix>(address, std::forward<TFunc>(func));
				}
				else
				{
					m_HeapBuffer = std::make_unique<Ix>(std::forward<TFunc>(func));
					m_Target = m_HeapBuffer.get();
				}
				return m_Target;
			}

			void DestroyTarget()
			{
				if (IsUsingHeapBuffer())
				{
					m_HeapBuffer.reset();
				}
				else if (IsUsingLocalBuffer())
				{
					m_Target->destroy_storage();
				}
				m_Target = nullptr;
			}

			template<class T>
			T* GetTargetImpl() const noexcept
			{
				return dynamic_cast<impl_t<T>*>(m_Target);
			}

			bool IsUsingHeapBuffer() const noexcept
			{
				return m_Target != nullptr && m_HeapBuffer != nullptr;
			}
			bool IsUsingLocalBuffer() const noexcept
			{
				return m_Target != nullptr && m_HeapBuffer == nullptr;
			}
			size_t GetLocalBufferOffset() const noexcept
			{
				return reinterpret_cast<size_t>(m_Target) - reinterpret_cast<size_t>(m_LocalBuffer.data());
			}

		public:
			unique_function() = default;
			unique_function(unique_function&& other) noexcept
			{
				assign(std::move(other));
			}
			unique_function(const unique_function&) = delete;

			template<class TFunc> requires(CheckTargetType<TFunc>())
			unique_function(TFunc&& func)
			{
				CreateTarget(std::forward<TFunc>(func));
			}

			~unique_function()
			{
				DestroyTarget();
			}

		public:
			bool is_null() const noexcept
			{
				return m_Target == nullptr;
			}

			void swap(unique_function& other) noexcept
			{
				std::swap(*this, other);
			}

			R invoke(Args&&... arg) const
			{
				return m_Target->invoke(std::forward<Args>(arg)...);
			}

			void assign(unique_function&& other) noexcept
			{
				if (this != &other)
				{
					DestroyTarget();

					if (other.IsUsingHeapBuffer())
					{
						m_HeapBuffer = std::move(other.m_HeapBuffer);
						m_Target = m_HeapBuffer.get();

						other.m_Target = nullptr;
					}
					else if (other.IsUsingLocalBuffer())
					{
						m_Target = std::launder(reinterpret_cast<interface_t*>(m_LocalBuffer.data() + other.GetLocalBufferOffset()));
						other.m_Target->move_storage(m_Target, std::move(*other.m_Target));
					}
					other.DestroyTarget();
				}
			}

			template<class TFunc> requires(CheckTargetType<TFunc>())
			void assign(TFunc&& func)
			{
				DestroyTarget();
				CreateTarget(std::forward<TFunc>(func));
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

			unique_function& operator=(unique_function&& other) noexcept
			{
				assign(std::move(other));
				return *this;
			}
			unique_function& operator=(const unique_function&) = delete;

			template<class TFunc> requires(CheckTargetType<TFunc>())
			unique_function& operator=(TFunc&& func)
			{
				assign(std::forward<TFunc>(func));
				return *this;
			}
	};
}
