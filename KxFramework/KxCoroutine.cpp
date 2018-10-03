#include "KxStdAfx.h"
#include "KxFramework/KxCoroutine.h"
#include <chrono>

namespace Util
{
	uint64_t GetClockTime()
	{
		using namespace std::chrono;
		return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
	}
}

class KxCoroutineCallData: public wxAsyncMethodCallEvent
{
	private:
		KxCoroutineBase* const m_Coroutine = NULL;

	public:
		KxCoroutineCallData()
			:wxAsyncMethodCallEvent(wxApp::GetInstance())
		{
		}
		KxCoroutineCallData(KxCoroutineBase* coroutine)
			:wxAsyncMethodCallEvent(wxApp::GetInstance()), m_Coroutine(coroutine)
		{
		}

	public:
		bool IsOK() const
		{
			return m_Coroutine != NULL;
		}
		
		virtual KxCoroutineCallData* Clone() const override
		{
			return new KxCoroutineCallData(*this);
		}
		virtual void Execute() override
		{
			m_Coroutine->UpdateCallData(this);
			m_Coroutine->BeforeExecute();
			m_Coroutine->RunExecute();
			m_Coroutine->AfterExecute();
		}
};

//////////////////////////////////////////////////////////////////////////
void KxCoroutineBase::Run(KxCoroutineBase* coroutine)
{
	QueueExecution(new KxCoroutineCallData(coroutine));
}
void KxCoroutineBase::Stop(KxCoroutineBase* coroutine)
{
	coroutine->m_ShouldStop = true;
	wxApp::GetInstance()->ScheduleForDestruction(coroutine);
}

void KxCoroutineBase::QueueExecution(KxCoroutineCallData* callData)
{
	wxApp::GetInstance()->QueueEvent(callData);
}
void KxCoroutineBase::DelayExecution(KxCoroutineCallData* callData)
{
	QueueExecution(callData->Clone());
}

void KxCoroutineBase::BeforeExecute()
{
	m_TimeStampBefore = GetExecutionTime();
	if (m_TimeStampAfter == 0)
	{
		m_TimeStampAfter = m_TimeStampBefore;
	}
}
void KxCoroutineBase::AfterExecute()
{
	m_TimeStampAfter = GetExecutionTime();
}
void KxCoroutineBase::RunExecute()
{
	switch (m_Enumerator)
	{
		case Enumerator::Wait:
		{
			if (ShouldExecuteAfter(m_TimeStampAfter))
			{
				m_ExecuteAfterTimePoint = 0;
				break;
			}
			if (ShouldExecuteAfterCompletion() && wxApp::GetInstance()->IsScheduledForDestruction(m_ExecuteAfterCoroutine))
			{
				NotifyCompletion();
			}

			DelayExecution(m_CallData);
			return;
		}
		case Enumerator::Terminate:
		{
			Stop(this);
			return;
		}
	};

	// For Enumerator::Continue;
	m_Enumerator = Execute();
	DelayExecution(m_CallData);
}
void KxCoroutineBase::NotifyCompletion()
{
	// Reset
	KxCoroutineBase* coroutine = m_ExecuteAfterCoroutine;
	m_ExecuteAfterCoroutine = NULL;

	// Call
	QueueExecution(coroutine->m_CallData);
}

uint64_t KxCoroutineBase::GetExecutionTime() const
{
	return Util::GetClockTime() - m_TimeStampStart;
}

KxCoroutineBase::KxCoroutineBase()
	:m_TimeStampStart(Util::GetClockTime())
{
}
KxCoroutineBase::~KxCoroutineBase()
{
}

uint64_t KxCoroutineBase::GetTimeDeltaMilliseconds() const
{
	return m_TimeStampBefore - m_TimeStampAfter;
}
uint64_t KxCoroutineBase::GetElapsedTimeMilliseconds() const
{
	return std::max(m_TimeStampBefore, m_TimeStampAfter);
}

KxCoroutineBase::Enumerator KxCoroutineBase::Yield()
{
	return Enumerator::Continue;
}
KxCoroutineBase::Enumerator KxCoroutineBase::YieldStop()
{
	return Enumerator::Terminate;
}
KxCoroutineBase::Enumerator KxCoroutineBase::YieldWaitMilliseconds(uint64_t timeMS)
{
	m_ExecuteAfterTimePoint = GetElapsedTimeMilliseconds() + timeMS;
	return Enumerator::Wait;
}
