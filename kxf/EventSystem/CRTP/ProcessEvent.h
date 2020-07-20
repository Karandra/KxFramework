#pragma once
#include "../Common.h"
#include "../Event.h"
#include "../EventBuilder.h"
#include "../EventExecutor.h"

namespace kxf::EventSystem
{
	template<class T>
	class ProcessEventCRTP
	{
		private:
			T& Self()
			{
				return static_cast<T&>(*this);
			}

		public:
			// ProcessEvent
			bool ProcessEvent(Event& event, const EventID& eventID = {})
			{
				return Self().DoProcessEvent(event, eventID);
			}

			template<class TEvent, class... Args>
			bool ProcessEvent(const EventID& eventID, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return Self().DoProcessEvent(event, eventID);
			}

			template<class TEvent, class... Args>
			bool ProcessEvent(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return Self().DoProcessEvent(event, eventTag);
			}

			// Processes an event by calling 'DoProcessEvent' and handles any exceptions that occur in the process
			bool ProcessEventSafely(Event& event, const EventID& eventID)
			{
				return Self().DoProcessEventSafely(event, eventID);
			}

			template<class TEvent, class... Args>
			bool ProcessEventSafely(const EventID& eventID, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return Self().DoProcessEventSafely(event, eventID);
			}

			template<class TEvent, class... Args>
			bool ProcessEventSafely(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return Self().DoProcessEventSafely(event, eventTag);
			}

			// Try to process the event in this handler and all those chained to it
			bool ProcessEventLocally(Event& event, const EventID& eventID)
			{
				return Self().DoProcessEventLocally(event, eventID);
			}

			template<class TEvent, class... Args>
			bool ProcessEventLocally(const EventID& eventID, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return Self().DoProcessEventLocally(event, eventID);
			}

			template<class TEvent, class... Args>
			bool ProcessEventLocally(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				TEvent event(std::forward<Args>(arg)...);
				return Self().DoProcessEventLocally(event, eventTag);
			}

			// Construct and send the event using the event builder
			template<class TEvent, class... Args>
			auto BuildProcessEvent(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				return EventSystem::DirectEventBuilder<TEvent>(Self(), TEvent(std::forward<Args>(arg)...), eventTag);
			}
	};
}
