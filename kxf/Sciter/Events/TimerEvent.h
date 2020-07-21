#pragma once
#include "Event.h"

namespace kxf::Sciter
{
	class KX_API TimerEvent: public Event
	{
		public:
			KxEVENT_MEMBER(TimerEvent, Timer);

		protected:
			uintptr_t m_TimerID = 0;

		public:
			TimerEvent(Host& host)
				:Event(host)
			{
			}

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<TimerEvent>(std::move(*this));
			}
			FlagSet<EventCategory> GetEventCategory() const override
			{
				return EventCategory::Timer;
			}

			uintptr_t GetTimerID() const
			{
				return m_TimerID;
			}
			void SetTimerID(uintptr_t value)
			{
				m_TimerID = value;
			}
	};
}

namespace kxf::Sciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(TimerEvent, Timer);
}
