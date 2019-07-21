#include "KxStdAfx.h"
#include "CoroutineImpl.h"
#include <chrono>

namespace
{
	wxTimeSpan GetClockTime()
	{
		using namespace std::chrono;
		return wxTimeSpan::Milliseconds(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
	}
}

namespace Kx::Async
{
	void CoroutineTimer::Notify()
	{
		BaseCoroutine::QueueExecution(std::move(m_Coroutine));
	}
	std::unique_ptr<BaseCoroutine> CoroutineTimer::Relinquish()
	{
		wxTimer::Stop();

		auto temp = std::move(m_Coroutine);
		return temp;
	}
	void CoroutineTimer::Wait(std::unique_ptr<BaseCoroutine> coroutine, const wxTimeSpan& time)
	{
		m_Coroutine = std::move(coroutine);
		wxTimer::StartOnce(time.GetMilliseconds().GetValue());
	}
}

namespace Kx::Async
{
	CoroutineExecutor::CoroutineExecutor(std::unique_ptr<BaseCoroutine> coroutine)
		:wxAsyncMethodCallEvent(wxApp::GetInstance()), m_Coroutine(std::move(coroutine))
	{
	}

	void CoroutineExecutor::Execute()
	{
		m_Coroutine->RunExecute(std::move(m_Coroutine));
	}
}

namespace Kx::Async
{
	BaseCoroutine* BaseCoroutine::Run(std::unique_ptr<BaseCoroutine> coroutine)
	{
		if (coroutine)
		{
			BaseCoroutine& ref = *coroutine;
			QueueExecution(std::move(coroutine));
			return &ref;
		}
		return nullptr;
	}

	void BaseCoroutine::QueueExecution(std::unique_ptr<BaseCoroutine> coroutine)
	{
		wxApp::GetInstance()->QueueEvent(new CoroutineExecutor(std::move(coroutine)));
	}
	void BaseCoroutine::DelayExecution(std::unique_ptr<BaseCoroutine> coroutine, const wxTimeSpan& time)
	{
		if (time.IsPositive())
		{
			BaseCoroutine& ref = *coroutine;
			ref.m_DelayTimer.Wait(std::move(coroutine), time);
		}
		else
		{
			QueueExecution(std::move(coroutine));
		}
	}
	void BaseCoroutine::AbortExecution(std::unique_ptr<BaseCoroutine> coroutine)
	{
		wxApp::GetInstance()->ScheduleForDestruction(coroutine.release());
	}

	void BaseCoroutine::BeforeExecute()
	{
		// Save starting time
		if (m_TimeStampStart.IsNull())
		{
			m_TimeStampStart = GetClockTime();
		}

		// Save before-after timestamps
		m_TimeStampBefore = GetCurrentExecutionTime();
		if (m_TimeStampAfter.IsNull())
		{
			m_TimeStampAfter = m_TimeStampBefore;
		}
	}
	void BaseCoroutine::AfterExecute()
	{
		m_TimeStampAfter = GetCurrentExecutionTime();
	}
	void BaseCoroutine::RunExecute(std::unique_ptr<BaseCoroutine> coroutine)
	{
		if (m_Instruction.GetType() == InstructionType::Terminate)
		{
			AbortExecution(std::move(coroutine));
			return;
		}

		BeforeExecute();
		m_Instruction = Execute();
		AfterExecute();

		switch (m_Instruction.GetType())
		{
			case InstructionType::Delay:
			{
				DelayExecution(std::move(coroutine), m_Instruction.GetDelay());
				break;
			}
			case InstructionType::Continue:
			{
				QueueExecution(std::move(coroutine));
				break;
			}
			default:
			{
				AbortExecution(std::move(coroutine));
				break;
			}
		};
	}

	wxTimeSpan BaseCoroutine::GetCurrentExecutionTime() const
	{
		return GetClockTime() - m_TimeStampStart;
	}

	BaseCoroutine::BaseCoroutine()
		:m_Instruction(InstructionType::Continue)
	{
	}
	BaseCoroutine::~BaseCoroutine()
	{
	}

	void BaseCoroutine::Terminate()
	{
		m_Instruction = BaseCoroutine::YieldStop();

		if (m_DelayTimer.IsRunning())
		{
			AbortExecution(m_DelayTimer.Relinquish());
		}
	}
	
	wxTimeSpan BaseCoroutine::GetTimeDelta() const
	{
		return m_TimeStampBefore - m_TimeStampAfter;
	}
	wxTimeSpan BaseCoroutine::GetElapsedTime() const
	{
		return std::max(m_TimeStampBefore, m_TimeStampAfter);
	}
}
