#pragma once
#include "Common.h"
#include "CFunctionCompiler.h"
#include "kxf/System/CFunctionCompiler.h"
#include <functional>

namespace kxf::FFI
{
	template<class TDerived, ABI ABI_, class TResult_, class... TArgTypes_>
	class CompiledCallableCRTP: protected CFunctionCompiler
	{
		public:
			using TCallableResult = typename TResult_;
			using TCallableSignature = typename TResult_(TArgTypes_...);
			using TFuncSignature = typename std::conditional_t<ABI_ == ABI::StdCall, TResult_(__stdcall)(TArgTypes_...), TResult_(__cdecl)(TArgTypes_...)>;

		protected:
			TDerived& GetDerived() noexcept
			{
				return static_cast<TDerived&>(*this);
			}
			const TDerived& GetDerived() const noexcept
			{
				return static_cast<const TDerived&>(*this);
			}

			void Init() noexcept
			{
				SetABI(ABI_);
				SetReturnType(GetTypeID<TCallableResult>());
				SetParameters({GetTypeID<TArgTypes_>()...});
				Create();
			}
			void Execute(void** arguments, void* returnValue) noexcept override
			{
				constexpr size_t argsCount = sizeof...(TArgTypes_);
				std::tuple<TArgTypes_...> args = ConvertToTuple(arguments, std::make_index_sequence<argsCount>());
				
				if constexpr(std::is_void_v<TCallableResult>)
				{
					GetDerived().ExecuteWith(std::move(args));
				}
				else
				{
					*(reinterpret_cast<TCallableResult*>(returnValue)) = static_cast<TCallableResult>(GetDerived().ExecuteWith(std::move(args)));
				}
			}

			template<size_t... sequence>
			static std::tuple<TArgTypes_...> ConvertToTuple(void** arguments, std::index_sequence<sequence...>) noexcept
			{
				return std::make_tuple(TArgTypes_{*reinterpret_cast<TArgTypes_*>(arguments[sequence])}...);
			}

		public:
			CompiledCallableCRTP() noexcept = default;
			CompiledCallableCRTP(const CompiledCallableCRTP&) = delete;
			CompiledCallableCRTP(CompiledCallableCRTP&&) noexcept = default;

		public:
			const CFunctionCompiler& GetCompiler() const noexcept
			{
				return *this;
			}
			CFunctionCompiler& GetCompiler() noexcept
			{
				return *this;
			}
			TFuncSignature* GetFunctionPointer() const noexcept
			{
				return reinterpret_cast<TFuncSignature*>(GetCode());
			}

			TCallableResult Invoke(TArgTypes_&&... arg) noexcept(std::is_nothrow_invocable_r_v<TCallableResult, TCallableSignature, TArgTypes_...>)
			{
				return GetDerived().ExecuteWith(std::make_tuple(std::forward<TArgTypes_>(arg)...));
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			TCallableResult operator()(TArgTypes_&&... arg) noexcept(std::is_nothrow_invocable_r_v<TCallableResult, TCallableSignature, TArgTypes_...>)
			{
				return Invoke(std::forward<TArgTypes_>(arg)...);
			}

			CompiledCallableCRTP& operator=(const CompiledCallableCRTP&) = delete;
			CompiledCallableCRTP& operator=(CompiledCallableCRTP&&) noexcept = default;
	};

	template<ABI ABI_, class TResult_, class... TArgTypes_>
	class CompiledCallable: public CompiledCallableCRTP<CompiledCallable<ABI_, TResult_, TArgTypes_...>, ABI_, TResult_, TArgTypes_...>
	{
		friend CompiledCallableCRTP;

		private:
			using TSelf = typename CompiledCallable;
			using TCallableResult = typename TSelf::TCallableResult;
			using TCallableSignature = typename TSelf::TCallableSignature;

		protected:
			std::move_only_function<TCallableSignature> m_Callable;

		protected:
			TCallableResult ExecuteWith(std::tuple<TArgTypes_...> args) noexcept
			{
				return std::apply(m_Callable, std::move(args));
			}

		public:
			CompiledCallable() noexcept = default;

			template<class TFunc> requires(std::is_constructible_v<decltype(m_Callable), TFunc>)
			CompiledCallable(TFunc&& func) noexcept(std::is_nothrow_constructible_v<decltype(m_Callable), TFunc>)
				:m_Callable(std::forward<TFunc>(func))
			{
				this->Init();
			}

		public:
			template<class TFunc> requires(std::is_assignable_v<decltype(m_Callable), TFunc>)
			void SetCallable(TFunc&& func) noexcept(std::is_nothrow_assignable_v<decltype(m_Callable), TFunc>)
			{
				m_Callable = std::forward<TFunc>(func);
				if (this->IsNull())
				{
					this->Init();
				}
			}

			TCallableResult Invoke(TArgTypes_&&... arg) noexcept(std::is_nothrow_invocable_r_v<TCallableResult, TCallableSignature, TArgTypes_...>)
			{
				return std::invoke(m_Callable, std::forward<TArgTypes_>(arg)...);
			}

		public:
			explicit operator bool() const noexcept
			{
				return m_Callable && !this->IsNull();
			}
			bool operator!() const noexcept
			{
				return !m_Callable || this->IsNull();
			}

			TCallableResult operator()(TArgTypes_&&... arg) noexcept(std::is_nothrow_invocable_r_v<TCallableResult, TCallableSignature, TArgTypes_...>)
			{
				return Invoke(std::forward<TArgTypes_>(arg)...);
			}
	};
}

namespace kxf::FFI::Private
{
	template<ABI ABI_, class>
	struct CompiledCallableWrapper;
	
	template<ABI ABI_, class TResult_, class... TArgTypes_>
	struct CompiledCallableWrapper<ABI_, TResult_(TArgTypes_...)>
	{
		using TCompiledCallable = CompiledCallable<ABI_, TResult_, TArgTypes_...>;
	};
}

namespace kxf
{
	template<class TSignature, FFI::ABI ABI_ = FFI::ABI::Default>
	requires(std::is_function_v<TSignature>)
	class CFunction: public FFI::Private::CompiledCallableWrapper<ABI_, TSignature>::TCompiledCallable
	{
		private:
			using TBase = typename FFI::Private::CompiledCallableWrapper<ABI_, TSignature>::TCompiledCallable;

		public:
			using TBase::TBase;
	};
}
