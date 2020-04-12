#pragma once
#include "../Common.h"
#include "Kx/System/CFunctionCompiler.h"
#include <limits>

namespace KxFramework::FFI::Private
{
	template<ABI t_ABI, class t_Ret, class... t_Types>
	class CompiledCallable: protected CFunctionCompiler
	{
		public:
			using TCallable = typename std::function<t_Ret(t_Types...)>;
			using TCallableResult = typename t_Ret;
			using TCallableSignature = typename t_Ret(t_Types...);
			using TFuncPtr = typename std::conditional_t<t_ABI == ABI::StdCall, t_Ret(__stdcall *)(t_Types...), t_Ret(__cdecl *)(t_Types...)>;

		protected:
			TCallable m_Callable;

		private:
			void Init() noexcept
			{
				SetABI(t_ABI);
				SetReturnType(GetTypeID<TCallableResult>());
				SetParameters({GetTypeID<t_Types>()...});
				Create();
			}
			void SetParameters(std::initializer_list<TypeID> types) noexcept
			{
				for (TypeID type: types)
				{
					AddParameter(type);
				}
			}
			
			template<size_t... t_Sequence>
			static std::tuple<t_Types...> ConvertToTuple(void** arguments, std::index_sequence<t_Sequence...>) noexcept
			{
				return std::make_tuple(t_Types {*reinterpret_cast<t_Types*>(arguments[t_Sequence])}...);
			}
			
			void Execute(void** arguments, void* returnValue) noexcept override
			{
				constexpr size_t argsCount = sizeof...(t_Types);
				std::tuple<t_Types...> args = ConvertToTuple(arguments, std::make_index_sequence<argsCount>());
				
				if constexpr(std::is_void_v<TCallableResult>)
				{
					std::apply(m_Callable, std::move(args));
				}
				else
				{
					*(reinterpret_cast<TCallableResult*>(returnValue)) = std::apply(m_Callable, args);
				}
			}

		public:
			CompiledCallable() noexcept
			{
				Init();
			}

			template<class TFunc>
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

		public:
			TCallableResult operator()(t_Types&&... arg)
			{
				return std::invoke(m_Callable, std::forward<t_Types>(arg)...);
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

	template<ABI t_ABI, class>
	struct CompiledCallableWrapper;
	
	template<ABI t_ABI, class t_Ret, class... t_Types>
	struct CompiledCallableWrapper<t_ABI, t_Ret(t_Types...)>
	{
		using TCompiledCallable = CompiledCallable<t_ABI, t_Ret, t_Types...>;
	};
}
