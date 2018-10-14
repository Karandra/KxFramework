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
		KxCoroutineBase* GetCoroutine() const
		{
			return m_Coroutine;
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
class KxCoroutineTimer: public wxTimer
{
	private:
		KxCoroutineBase* const m_Coroutine = NULL;

	public:
		KxCoroutineTimer(KxCoroutineBase* coroutine)
			:m_Coroutine(coroutine)
		{
		}
	
	public:
		virtual void Notify() override
		{
			m_Coroutine->QueueExecution();
		}
		bool RunTimer(uint64_t time)
		{
			// Copy call data as it will be deleted after timer is started
			m_Coroutine->CloneCallData();

			// Setup next call
			m_Coroutine->m_ExecuteAfterTimeInterval = 0;
			m_Coroutine->m_Enumerator = KxCoroutineBase::Enumerator::Continue;

			return wxTimer::StartOnce(time);
		}
};

//////////////////////////////////////////////////////////////////////////
void KxCoroutineBase::Run(KxCoroutineBase* coroutine)
{
	coroutine->UpdateCallData(new KxCoroutineCallData(coroutine));
	coroutine->QueueExecution();
}
void KxCoroutineBase::Stop(KxCoroutineBase* coroutine)
{
	coroutine->m_ShouldStop = true;
	wxApp::GetInstance()->ScheduleForDestruction(coroutine);
}

void KxCoroutineBase::QueueExecution()
{
	wxApp::GetInstance()->QueueEvent(m_CallData);
}
void KxCoroutineBase::DelayExecution()
{
	UpdateCallData(m_CallData->Clone());
	QueueExecution();
}
void KxCoroutineBase::DelayExecution(uint64_t timeMS)
{
	if (!m_Timer)
	{
		m_Timer = new KxCoroutineTimer(this);
	}
	m_Timer->RunTimer(m_ExecuteAfterTimeInterval);
}

void KxCoroutineBase::BeforeExecute()
{
	if (m_TimeStampStart == 0)
	{
		m_TimeStampStart = Util::GetClockTime();
	}

	m_TimeStampBefore = GetCurrentExecutionTime();
	if (m_TimeStampAfter == 0)
	{
		m_TimeStampAfter = m_TimeStampBefore;
	}
}
void KxCoroutineBase::AfterExecute()
{
	m_TimeStampAfter = GetCurrentExecutionTime();
}
void KxCoroutineBase::RunExecute()
{
	if (ShouldStop())
	{
		m_Enumerator = Enumerator::Terminate;
	}

	switch (m_Enumerator)
	{
		case Enumerator::Wait:
		{
			if (ShouldExecuteAfter())
			{
				DelayExecution(m_ExecuteAfterTimeInterval);
				return;
			}

			DelayExecution();
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
	DelayExecution();
}

void KxCoroutineBase::UpdateCallData(KxCoroutineCallData* callData)
{
	m_CallData = callData;
}
void KxCoroutineBase::CloneCallData()
{
	m_CallData = m_CallData->Clone();
}

uint64_t KxCoroutineBase::GetCurrentExecutionTime() const
{
	return Util::GetClockTime() - m_TimeStampStart;
}

KxCoroutineBase::KxCoroutineBase()
{
}
KxCoroutineBase::~KxCoroutineBase()
{
	delete m_Timer;
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
	m_ExecuteAfterTimeInterval = timeMS;
	return Enumerator::Wait;
}
