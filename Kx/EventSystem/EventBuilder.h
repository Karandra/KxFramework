#pragma once
#include "Event.h"
#include <optional>
class KxBasicEvtHandler;

namespace Kx::EventSystem
{
	class KX_API EventBuilder
	{
		protected:
			KxBasicEvtHandler* m_EvtHandler = nullptr;
			wxEvent* m_Event = nullptr;
			std::optional<KxEventID> m_EventID;

			bool m_IsNotifyEvent = false;
			bool m_IsAsync = false;
			bool m_IsSent = false;
			bool m_IsSkipped = false;
			bool m_IsAllowed = true;
			bool m_IsProcessed = false;

		private:
			EventBuilder() = default;

		public:
			EventBuilder(KxBasicEvtHandler& evtHandler, std::unique_ptr<wxEvent> event, std::optional<KxEventID> eventID = {})
				:m_EvtHandler(&evtHandler), m_Event(event.release()), m_EventID(eventID), m_IsAsync(true)
			{
			}
			EventBuilder(KxBasicEvtHandler& evtHandler, wxEvent& event, std::optional<KxEventID> eventID = {})
				:m_EvtHandler(&evtHandler), m_Event(&event), m_EventID(eventID), m_IsAsync(false)
			{
			}
			EventBuilder(EventBuilder&& other)
			{
				*this = std::move(other);
			}
			EventBuilder(const EventBuilder&) = delete;
			virtual ~EventBuilder();

		public:
			EventBuilder& Do();

			bool IsAsync() const
			{
				return m_IsAsync;
			}
			bool IsSkipped() const
			{
				return m_IsSkipped;
			}
			bool IsAllowed() const
			{
				return m_IsAllowed;
			}
			bool IsProcessed() const
			{
				return m_IsProcessed;
			}

		public:
			EventBuilder& operator=(EventBuilder&& other);
			EventBuilder& operator=(const EventBuilder&) = delete;
	};
}

template<class TEvent>
class KxEventBuilder: public Kx::EventSystem::EventBuilder
{
	private:
		void TestEventClass()
		{
			m_IsNotifyEvent = std::is_base_of_v<wxNotifyEvent, TEvent>;
		}

	public:
		KxEventBuilder(KxBasicEvtHandler& evtHandler, std::unique_ptr<TEvent> event, std::optional<KxEventID> eventID = {})
			:EventBuilder(evtHandler, std::move(event), eventID)
		{
			TestEventClass();
		}
		KxEventBuilder(KxBasicEvtHandler& evtHandler, TEvent& event, std::optional<KxEventID> eventID = {})
			:EventBuilder(evtHandler, event, eventID)
		{
			TestEventClass();
		}
		KxEventBuilder(KxEventBuilder&& other)
			:EventBuilder(std::move(other))
		{
		}

	public:
		template<class TFunctor> KxEventBuilder& On(TFunctor&& func)
		{
			std::invoke(func, static_cast<TEvent&>(*m_Event));
			return *this;
		}
		KxEventBuilder& Do()
		{
			EventBuilder::Do();
			return *this;
		}

	public:
		KxEventBuilder& operator=(KxEventBuilder&& other)
		{
			static_cast<EventBuilder&>(*this) = std::move(other);
			return *this;
		}
};
