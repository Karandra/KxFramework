#include "stdafx.h"
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

namespace KxFramework::Async
{
	void CoroutineTimer::Notify()
	{
		CoroutineBase::QueueExecution(std::move(m_Coroutine));
	}
	std::unique_ptr<CoroutineBase> CoroutineTimer::Relinquish()
	{
		wxTimer::Stop();
		return std::move(m_Coroutine);
	}
	void CoroutineTimer::Wait(std::unique_ptr<CoroutineBase> coroutine, const wxTimeSpan& time)
	{
		m_Coroutine = std::move(coroutine);
		wxTimer::StartOnce(time.GetMilliseconds().GetValue());
	}
}

namespace KxFramework::Async
{
	CoroutineExecutor::CoroutineExecutor(std::unique_ptr<CoroutineBase> coroutine)
		:wxAsyncMethodCallEvent(wxApp::GetInstance()), m_Coroutine(std::move(coroutine))
	{
	}

	void CoroutineExecutor::Execute()
	{
		m_Coroutine->RunExecute(std::move(m_Coroutine));
	}
}

namespace KxFramework::Async
{
	CoroutineBase* CoroutineBase::Run(std::unique_ptr<CoroutineBase> coroutine)
	{
		if (coroutine)
		{
			CoroutineBase& ref = *coroutine;
			QueueExecution(std::move(coroutine));
			return &ref;
		}
		return nullptr;
	}

	void CoroutineBase::QueueExecution(std::unique_ptr<CoroutineBase> coroutine)
	{
		wxApp::GetInstance()->QueueEvent(new CoroutineExecutor(std::move(coroutine)));
	}
	void CoroutineBase::DelayExecution(std::unique_ptr<CoroutineBase> coroutine, const wxTimeSpan& time)
	{
		if (time.IsPositive())
		{
			CoroutineBase& ref = *coroutine;
			ref.m_DelayTimer.Wait(std::move(coroutine), time);
		}
		else
		{
			QueueExecution(std::move(coroutine));
		}
	}
	void CoroutineBase::AbortExecution(std::unique_ptr<CoroutineBase> coroutine)
	{
		wxApp::GetInstance()->ScheduleForDestruction(coroutine.release());
	}

	void CoroutineBase::BeforeExecute()
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
	void CoroutineBase::AfterExecute()
	{
		m_TimeStampAfter = GetCurrentExecutionTime();
	}
	void CoroutineBase::RunExecute(std::unique_ptr<CoroutineBase> coroutine)
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

	wxTimeSpan CoroutineBase::GetCurrentExecutionTime() const
	{
		return GetClockTime() - m_TimeStampStart;
	}

	CoroutineBase::CoroutineBase()
		:m_Instruction(InstructionType::Continue)
	{
	}
	CoroutineBase::~CoroutineBase()
	{
	}

	void CoroutineBase::Terminate()
	{
		m_Instruction = CoroutineBase::YieldStop();

		if (m_DelayTimer.IsRunning())
		{
			AbortExecution(m_DelayTimer.Relinquish());
		}
	}
	
	wxTimeSpan CoroutineBase::GetTimeDelta() const
	{
		return m_TimeStampBefore - m_TimeStampAfter;
	}
	wxTimeSpan CoroutineBase::GetElapsedTime() const
	{
		return std::max(m_TimeStampBefore, m_TimeStampAfter);
	}
}
