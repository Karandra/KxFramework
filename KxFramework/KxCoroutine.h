/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include <utility>
#include <optional>
#include <type_traits>

class KX_API KxCoroutine;
namespace Kx::Async
{
	class KX_API BaseCoroutine;
	enum class InstructionType
	{
		Delay,
		Continue,
		Terminate,
	};
}

namespace Kx::Async
{
	template<class T> void AssertStateType()
	{
		static_assert(std::is_integral_v<T> || std::is_enum_v<T>);
	}
}

namespace Kx::Async
{
	class CoroutineTimer: public wxTimer
	{
		private:
			std::unique_ptr<BaseCoroutine> m_Coroutine;

		public:
			void Notify() override;
			void Wait(std::unique_ptr<BaseCoroutine> coroutine, const wxTimeSpan& time);
			std::unique_ptr<BaseCoroutine> Relinquish();
	};
	class CoroutineExecutor: public wxAsyncMethodCallEvent
	{
		private:
			std::unique_ptr<BaseCoroutine> m_Coroutine;

		public:
			CoroutineExecutor(std::unique_ptr<BaseCoroutine> coroutine);

		public:
			CoroutineExecutor* Clone() const override
			{
				return nullptr;
			}
			void Execute() override;
	};
}

class KX_API KxYieldInstruction
{
	friend class Kx::Async::BaseCoroutine;
	friend class KxCoroutine;

	private:
		using InstructionType = Kx::Async::InstructionType;

	private:
		InstructionType m_Type = InstructionType::Continue;
		wxTimeSpan m_Delay;
		std::optional<intptr_t> m_NextState;

	protected:
		KxYieldInstruction(InstructionType instruction) noexcept
			:m_Type(instruction)
		{
		}
		template<class T> KxYieldInstruction(InstructionType type, const T& nextState) noexcept
			:m_Type(type), m_NextState(static_cast<intptr_t>(nextState))
		{
			Kx::Async::AssertStateType<T>();
		}

	public:
		InstructionType GetType() const noexcept
		{
			return m_Type;
		}
		wxTimeSpan GetDelay() const
		{
			return m_Delay;
		}
		
		template<class T = intptr_t> std::optional<T> GetNextState() const noexcept
		{
			Kx::Async::AssertStateType<T>();
			if (m_NextState)
			{
				return static_cast<T>(*m_NextState);
			}
			else
			{
				return std::nullopt;
			}
		}
};

namespace Kx::Async
{
	class KX_API BaseCoroutine: public wxObject
	{
		friend class CoroutineTimer;
		friend class CoroutineExecutor;

		public:
			static BaseCoroutine* Run(std::unique_ptr<BaseCoroutine> coroutine);

			template<class T = intptr_t> static KxYieldInstruction Yield(const T& nextState = 0)
			{
				return KxYieldInstruction(InstructionType::Continue, nextState);
			}
			template<class T = intptr_t> static KxYieldInstruction YieldWait(const wxTimeSpan& interval, const T& nextState = 0)
			{
				KxYieldInstruction instruction(InstructionType::Delay, nextState);
				instruction.m_Delay = interval;
				return instruction;
			}
			template<class T = intptr_t> static KxYieldInstruction YieldStop(const T& nextState = 0)
			{
				return KxYieldInstruction(InstructionType::Terminate, nextState);
			}

		private:
			static void QueueExecution(std::unique_ptr<BaseCoroutine> coroutine);
			static void DelayExecution(std::unique_ptr<BaseCoroutine> coroutine, const wxTimeSpan& time);
			static void AbortExecution(std::unique_ptr<BaseCoroutine> coroutine);

		private:
			CoroutineTimer m_DelayTimer;
			KxYieldInstruction m_Instruction;
			wxTimeSpan m_TimeStampStart;
			wxTimeSpan m_TimeStampBefore;
			wxTimeSpan m_TimeStampAfter;

		private:
			void BeforeExecute();
			void AfterExecute();
			void RunExecute(std::unique_ptr<BaseCoroutine> coroutine);

			wxTimeSpan GetCurrentExecutionTime() const;

		protected:
			virtual KxYieldInstruction Execute() = 0;

		public:
			BaseCoroutine();
			virtual ~BaseCoroutine();

		public:
			void Terminate();

			template<class T = intptr_t> std::optional<T> GetNextState() const noexcept
			{
				return m_Instruction.GetNextState<T>();
			}

			wxTimeSpan GetTimeDelta() const;
			wxTimeSpan GetElapsedTime() const;
	};
}

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
