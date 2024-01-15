#pragma once
#include "Common.h"
#include "Private/CFunction.h"
#include "CFunctionCompiler.h"

namespace kxf
{
	template<class TSignature, FFI::ABI ABI_ = FFI::ABI::Default>
	class CFunction: public FFI::Private::CompiledCallableWrapper<ABI_, TSignature>::TCompiledCallable
	{
		private:
			using TBase = typename FFI::Private::CompiledCallableWrapper<ABI_, TSignature>::TCompiledCallable;

		public:
			CFunction() noexcept = default;

			template<class TFunc> requires(std::is_constructible_v<TBase, TFunc>)
			CFunction(TFunc&& func)
				:TBase(std::forward<TFunc>(func))
			{
			}

			CFunction(const CFunction&) = default;
			CFunction(CFunction&&) noexcept = default;

		public:
			template<class TFunc> requires(std::is_constructible_v<TBase, TFunc>)
			void SetCallable(TFunc&& func)
			{
				TBase::SetCallable(std::forward<TFunc>(func));
			}

			CFunction& operator=(const CFunction&) = default;
			CFunction& operator=(CFunction&&) noexcept = default;
	};
}
