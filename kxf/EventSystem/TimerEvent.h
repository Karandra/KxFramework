#pragma once
#include "Common.h"
#include "Event.h"
#include "kxf/Core/DateTime.h"

namespace kxf
{
	class ITimer;
}

namespace kxf
{
	class KX_API TimerEvent: public BasicEvent
	{
		public:
			KxEVENT_MEMBER(TimerEvent, Notify);

		private:
			ITimer* m_Timer = nullptr;
			TimeSpan m_Interval;
			int m_ID = -1;

		public:
			TimerEvent() = default;
			TimerEvent(TimeSpan interval, int id = -1)
				:m_Interval(interval), m_ID(id)
			{
			}
			TimerEvent(ITimer& timer, TimeSpan interval, int id = -1)
				:m_Timer(&timer), m_Interval(interval), m_ID(id)
			{
			}

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<TimerEvent>(std::move(*this));
			}

			ITimer* GetTimer() const noexcept
			{
				return m_Timer;
			}
			TimeSpan GetInterval() const noexcept
			{
				return m_Interval;
			}
			int GetID() const noexcept
			{
				return m_ID;
			}
	};
}
