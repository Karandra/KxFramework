#pragma once
#include "Kx/Async/Common.h"
#include "YieldInstruction.h"
#include <wx/timer.h>
#include <utility>
#include <optional>
#include <type_traits>

namespace KxFramework
{
	class KX_API Coroutine;
}
namespace KxFramework::Async
{
	class KX_API CoroutineBase;
}

namespace KxFramework::Async
{
	class CoroutineTimer final: public wxTimer
	{
		private:
			std::unique_ptr<CoroutineBase> m_Coroutine;

		public:
			void Notify() override;
			void Wait(std::unique_ptr<CoroutineBase> coroutine, const wxTimeSpan& time);
			std::unique_ptr<CoroutineBase> Relinquish();
	};
	class CoroutineExecutor final: public wxAsyncMethodCallEvent
	{
		private:
			std::unique_ptr<CoroutineBase> m_Coroutine;

		public:
			CoroutineExecutor(std::unique_ptr<CoroutineBase> coroutine);

		public:
			CoroutineExecutor* Clone() const override
			{
				return nullptr;
			}
			void Execute() override;
	};
}

namespace KxFramework::Async
{
	class KX_API CoroutineBase: public wxObject
	{
		friend class CoroutineTimer;
		friend class CoroutineExecutor;

		public:
			static CoroutineBase* Run(std::unique_ptr<CoroutineBase> coroutine);

			template<class T = intptr_t>
			static YieldInstruction Yield(const T& nextState = 0)
			{
				return YieldInstruction(InstructionType::Continue, nextState);
			}
			
			template<class T = intptr_t>
			static YieldInstruction YieldWait(const wxTimeSpan& interval, const T& nextState = 0)
			{
				YieldInstruction instruction(InstructionType::Delay, nextState);
				instruction.m_Delay = interval;
				return instruction;
			}
			
			template<class T = intptr_t>
			static YieldInstruction YieldStop(const T& nextState = 0)
			{
				return YieldInstruction(InstructionType::Terminate, nextState);
			}

		private:
			static void QueueExecution(std::unique_ptr<CoroutineBase> coroutine);
			static void DelayExecution(std::unique_ptr<CoroutineBase> coroutine, const wxTimeSpan& time);
			static void AbortExecution(std::unique_ptr<CoroutineBase> coroutine);

		private:
			CoroutineTimer m_DelayTimer;
			YieldInstruction m_Instruction;
			wxTimeSpan m_TimeStampStart;
			wxTimeSpan m_TimeStampBefore;
			wxTimeSpan m_TimeStampAfter;

		private:
			void BeforeExecute();
			void AfterExecute();
			void RunExecute(std::unique_ptr<CoroutineBase> coroutine);

			wxTimeSpan GetCurrentExecutionTime() const;

		protected:
			virtual YieldInstruction Execute() = 0;

		public:
			CoroutineBase();
			virtual ~CoroutineBase();

		public:
			void Terminate();

			template<class T = intptr_t>
			std::optional<T> GetNextState() const noexcept
			{
				return m_Instruction.GetNextState<T>();
			}

			wxTimeSpan GetTimeDelta() const;
			wxTimeSpan GetElapsedTime() const;
	};
}
