#pragma once
#include "Kx/Async/Common.h"
#include "CoroutineImpl.h"
#include "CoroutineInstruction.h"

class KX_API KxCoroutine: public Kx::Async::BaseCoroutine
{
	private:
		static KxCoroutine* DoRun(std::unique_ptr<KxCoroutine> coroutine)
		{
			return static_cast<KxCoroutine*>(BaseCoroutine::Run(std::move(coroutine)));
		}

	public:
		static KxCoroutine* Run(std::unique_ptr<KxCoroutine> coroutine)
		{
			return DoRun(std::move(coroutine));
		}
		template<class TCallable> static KxCoroutine* Run(TCallable func)
		{
			return DoRun(std::make_unique<Kx::Async::CoroutineCallableWrapper<TCallable>>(std::move(func)));
		}
		template<class TClass> static KxCoroutine* Run(KxYieldInstruction(TClass::*method)(KxCoroutine&), TClass* object)
		{
			return DoRun(std::make_unique<Kx::Async::CoroutineMethodWrapper<TClass>>(method, object));
		}
};

namespace Kx::Async
{
	template<class TCallable>
	class CoroutineCallableWrapper: public KxCoroutine
	{
		private:
			TCallable m_Callable;

		protected:
			KxYieldInstruction Execute() override
			{
				return std::invoke(m_Callable, *this);
			}

		public:
			CoroutineCallableWrapper(TCallable func)
				:m_Callable(std::move(func))
			{
			}
	};

	template<class TClass>
	class CoroutineMethodWrapper: public KxCoroutine
	{
		public:
			using TMethod = KxYieldInstruction(TClass::*)(KxCoroutine&);

		private:
			TClass* m_Object = nullptr;
			TMethod m_Method = nullptr;

		protected:
			KxYieldInstruction Execute() override
			{
				return std::invoke(m_Method, m_Object, *this);
			}

		public:
			CoroutineMethodWrapper(TMethod method, TClass* object)
				:m_Method(method), m_Object(object)
			{
			}
	};
}
