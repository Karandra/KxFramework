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
			TimerEvent* Clone() const override
			{
				return new TimerEvent(*this);
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
