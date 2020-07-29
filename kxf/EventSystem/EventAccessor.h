#pragma once
#include "Common.h"
#include "IEvent.h"

namespace kxf::EventSystem
{
	class EventAccessor final
	{
		private:
			IEvent& m_Event;

		public:
			EventAccessor(IEvent& event)
				:m_Event(event)
			{
			}

		public:
			bool WasQueueed() const
			{
				return m_Event.WasQueueed();
			}
			bool WasProcessed() const
			{
				return m_Event.WasProcessed();
			}
			bool WillBeProcessedAgain() const
			{
				return m_Event.WillBeProcessedAgain();
			}

			void OnStartProcess(const EventID& eventID, const UniversallyUniqueID& uuid)
			{
				m_Event.OnStartProcess(eventID, uuid);
			}
	};
}
