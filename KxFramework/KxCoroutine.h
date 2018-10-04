#pragma once
#include "KxFramework/KxFramework.h"
#include <utility>
#include <type_traits>
class KxCoroutineCallData;
class KxCoroutineTimer;

class KxCoroutineBase: public wxObject
{
	friend class KxCoroutineCallData;
	friend class KxCoroutineTimer;

	protected:
		enum class Enumerator
		{
			Wait,
			Continue,
			Terminate,
		};

	private:
		KxCoroutineCallData* m_CallData = NULL;
		KxCoroutineTimer* m_Timer = NULL;
		Enumerator m_Enumerator = Enumerator::Continue;
		uint64_t m_TimeStampStart = 0;
		uint64_t m_TimeStampBefore = 0;
		uint64_t m_TimeStampAfter = 0;

		bool m_ShouldStop = false;
		uint64_t m_ExecuteAfterTimePoint = 0;
		KxCoroutineBase* m_ExecuteAfterCoroutine = NULL;

	public:
		static void Run(KxCoroutineBase* coroutine);
		static void Stop(KxCoroutineBase* coroutine);

	private:
		static void QueueExecution(KxCoroutineCallData* callData);
		static void DelayExecution(KxCoroutineCallData* callData);

	private:
		void BeforeExecute();
		void AfterExecute();
		void RunExecute();
		void NotifyCompletion();
		void UpdateCallData(KxCoroutineCallData* callData)
		{
			m_CallData = callData;
		}

		uint64_t GetCurrentExecutionTime() const;

	protected:
		virtual Enumerator Execute() = 0;

		bool ShouldStop() const
		{
			return m_ShouldStop;
		}
		bool ShouldExecuteAfter() const
		{
			return m_ExecuteAfterTimePoint != 0;
		}
		bool ShouldExecuteAfterCompletion() const
		{
			return m_ExecuteAfterCoroutine != NULL;
		}
		bool ShouldNotifyCompletion() const
		{
			return m_ExecuteAfterCoroutine != NULL;
		}

	public:
		KxCoroutineBase();
		virtual ~KxCoroutineBase();

	public:
		uint64_t GetTimeDeltaMilliseconds() const;
		double GetTimeDeltaDeconds() const
		{
			return GetTimeDeltaMilliseconds() / 1000.0;
		}

		uint64_t GetElapsedTimeMilliseconds() const;
		double GetElapsedTimeSeconds() const
		{
			return GetElapsedTimeMilliseconds() / 1000.0;
		}

		Enumerator Yield();
		Enumerator YieldStop();

		#if 0
		Enumerator YieldWaitForCoroutine(KxCoroutineBase& coroutine)
		{
			m_ExecuteAfterCoroutine = &coroutine;
			return Enumerator::Wait;
		}
		#endif

		Enumerator YieldWaitMilliseconds(uint64_t timeMS);
		Enumerator YieldWaitSeconds(double timeSec)
		{
			return YieldWaitMilliseconds(timeSec * 1000.0);
		}
};

//////////////////////////////////////////////////////////////////////////
template<class T> class KxCoroutineFunctor: public KxCoroutineBase
{
	private:
		T m_Functor;

	protected:
		virtual Enumerator Execute() override
		{
			return std::invoke(m_Functor, *this);
		}

	public:
		KxCoroutineFunctor(const T& functor)
			:m_Functor(functor)
		{
		}
		KxCoroutineFunctor(T&& functor)
			:m_Functor(std::move(functor))
		{
		}
};

class KxCoroutine: public KxCoroutineBase
{
	public:
		static KxCoroutineBase* Run(KxCoroutineBase* coroutine)
		{
			KxCoroutineBase::Run(coroutine);
			return coroutine;
		}
		template<class T> static KxCoroutineFunctor<T>* Run(const T& coroutine)
		{
			auto functor = new KxCoroutineFunctor<T>(coroutine);
			KxCoroutineBase::Run(functor);
			return functor;
		}
		template<class T> static KxCoroutineFunctor<T>* Run(T&& coroutine)
		{
			auto functor = new KxCoroutineFunctor<T>(std::move(coroutine));
			KxCoroutineBase::Run(functor);
			return functor;
		}
};
