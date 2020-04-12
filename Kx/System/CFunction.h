#pragma once
#include "Common.h"
#include "Private/CFunction.h"
#include "CFunctionCompiler.h"

namespace KxFramework
{
	template<class TSignature, FFI::ABI t_ABI>
	class CFunction: public FFI::Private::CompiledCallableWrapper<t_ABI, TSignature>::TCompiledCallable
	{
		private:
			using TBase = typename FFI::Private::CompiledCallableWrapper<t_ABI, TSignature>::TCompiledCallable;

		public:
			CFunction() noexcept = default;

			template<class TFunc>
			CFunction(TFunc&& func)
				:TBase(std::forward<TFunc>(func))
			{
			}

			CFunction(const CFunction&) = default;
			CFunction(CFunction&&) noexcept = default;

		public:
			template<class TFunc>
			CFunction& operator=(TFunc&& func)
			{
				this->m_Functor = std::forward<TFunc>(func);
				return *this;
			}

			CFunction& operator=(const CFunction&) = default;
			CFunction& operator=(CFunction&&) noexcept = default;
	};
}
