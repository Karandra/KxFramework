#pragma once
#include "../Common.h"
#include "../Event.h"
#include "../EventBuilder.h"
#include "../EventExecutor.h"

namespace kxf::EventSystem
{
	template<class T>
	class QueueEventCRTP
	{
		private:
			T& Self()
			{
				return static_cast<T&(*this);
			}

		public:
			// Queue event for a later processing
			void QueueEvent(std::unique_ptr<Event> event, const EventID& eventID = {}, UniversallyUniqueID uuid = {})
			{
				Self().DoQueueEvent(std::move(event), eventID, std::move(uuid));
			}

			template<class TEvent, class... Args>
			void QueueEvent(const EventID& eventID, Args&&... arg)
			{
				Self().DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...), eventID);
			}

			template<class TEvent, class... Args>
			void QueueEvent(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				Self().DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...), eventTag);
			}

			template<class TEvent, class... Args>
			void QueueUniqueEvent(UniversallyUniqueID uuid, const EventID& eventID, Args&&... arg)
			{
				Self().DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...), eventID, std::move(uuid));
			}

			template<class TEvent, class... Args>
			void QueueUniqueEvent(UniversallyUniqueID uuid, const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				Self().DoQueueEvent(std::make_unique<TEvent>(std::forward<Args>(arg)...), eventTag, std::move(uuid));
			}

			// Construct and queue event using the event builder
			template<class TEvent, class... Args>
			auto BuildQueueEvent(const EventTag<TEvent>& eventTag, Args&&... arg)
			{
				return QueuedEventBuilder<TEvent>(Self(), std::make_unique<TEvent>(std::forward<Args>(arg)...), eventTag);
			}
	};
}
