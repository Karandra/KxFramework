#pragma once
#include "Kx/Async/Common.h"
#include "CoroutineInstruction.h"
#include <wx/timer.h>
#include <utility>
#include <optional>
#include <type_traits>

class KX_API KxCoroutine;
namespace KxAsync
{
	class KX_API BaseCoroutine;
}

namespace KxAsync
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

namespace KxAsync
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
