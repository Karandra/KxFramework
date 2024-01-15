#pragma once
#include "../Common.h"
#include "kxf/System/CFunctionCompiler.h"
#include <limits>
#include <functional>

namespace kxf::FFI::Private
{
	template<ABI ABI_, class TResult_, class... TArgTypes_>
	class CompiledCallable: protected CFunctionCompiler
	{
		public:
			using TCallable = typename std::move_only_function<TResult_(TArgTypes_...)>;
			using TCallableResult = typename TResult_;
			using TCallableSignature = typename TResult_(TArgTypes_...);
			using TFuncPtr = typename std::conditional_t<ABI_ == ABI::StdCall, TResult_(__stdcall *)(TArgTypes_...), TResult_(__cdecl *)(TArgTypes_...)>;

		protected:
			TCallable m_Callable;

		private:
			void Init() noexcept
			{
				SetABI(ABI_);
				SetReturnType(GetTypeID<TCallableResult>());
				SetParameters({GetTypeID<TArgTypes_>()...});
				Create();
			}
			void SetParameters(std::initializer_list<TypeID> types) noexcept
			{
				for (TypeID type: types)
				{
					AddParameter(type);
				}
			}
			
			template<size_t... sequence>
			static std::tuple<TArgTypes_...> ConvertToTuple(void** arguments, std::index_sequence<sequence...>) noexcept
			{
				return std::make_tuple(TArgTypes_ {*reinterpret_cast<TArgTypes_*>(arguments[sequence])}...);
			}
			
			void Execute(void** arguments, void* returnValue) noexcept override
			{
				constexpr size_t argsCount = sizeof...(TArgTypes_);
				std::tuple<TArgTypes_...> args = ConvertToTuple(arguments, std::make_index_sequence<argsCount>());
				
				if constexpr(std::is_void_v<TCallableResult>)
				{
					std::apply(m_Callable, std::move(args));
				}
				else
				{
					*(reinterpret_cast<TCallableResult*>(returnValue)) = static_cast<TCallableResult>(std::apply(m_Callable, std::move(args)));
				}
			}

		public:
			CompiledCallable() noexcept = default;

			template<class TFunc> requires(std::is_invocable_r_v<TCallableResult, TFunc, TArgTypes_...>)
			CompiledCallable(TFunc&& func)
				:m_Callable(std::forward<TFunc>(func))
			{
				Init();
			}

			CompiledCallable(const CompiledCallable&) = default;
			CompiledCallable(CompiledCallable&&) noexcept = default;

		public:
			const CFunctionCompiler& GetCompiler() const noexcept
			{
				return *this;
			}
			CFunctionCompiler& GetCompiler() noexcept
			{
				return *this;
			}

			TFuncPtr GetFunctionPointer() const noexcept
			{
				return reinterpret_cast<TFuncPtr>(GetCode());
			}

			template<class TFunc> requires(std::is_invocable_r_v<TCallableResult, TFunc, TArgTypes_...>)
			void SetCallable(TFunc&& func)
			{
				m_Callable = std::forward<TFunc>(func);
				if (!IsCreated())
				{
					Init();
				}
			}

		public:
			TCallableResult operator()(TArgTypes_&&... arg)
			{
				return std::invoke(m_Callable, std::forward<TArgTypes_>(arg)...);
			}
			operator TFuncPtr() const noexcept
			{
				return GetFunctionPointer();
			}

			explicit operator bool() const noexcept
			{
				return m_Callable && IsCreated();
			}
			bool operator!() const noexcept
			{
				return !m_Callable || !IsCreated();
			}

			CompiledCallable& operator=(const CompiledCallable&) = default;
			CompiledCallable& operator=(CompiledCallable&&) noexcept = default;
	};

	template<ABI ABI_, class>
	struct CompiledCallableWrapper;
	
	template<ABI ABI_, class TResult_, class... TArgTypes_>
	struct CompiledCallableWrapper<ABI_, TResult_(TArgTypes_...)>
	{
		using TCompiledCallable = CompiledCallable<ABI_, TResult_, TArgTypes_...>;
	};
}
