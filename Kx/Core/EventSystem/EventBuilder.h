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
			bool m_Async = false;
			bool m_Sent = false;
			bool m_IsSkipped = false;
			bool m_IsAllowed = true;

		private:
			EventBuilder() = default;

		public:
			EventBuilder(KxBasicEvtHandler& evtHandler, std::unique_ptr<wxEvent> event, std::optional<KxEventID> eventID = {})
				:m_EvtHandler(&evtHandler), m_Event(event.release()), m_EventID(eventID), m_Async(true)
			{
			}
			EventBuilder(KxBasicEvtHandler& evtHandler, wxEvent& event, std::optional<KxEventID> eventID = {})
				:m_EvtHandler(&evtHandler), m_Event(&event), m_EventID(eventID), m_Async(false)
			{
			}
			EventBuilder(EventBuilder&& other)
			{
				*this = std::move(other);
			}
			EventBuilder(const EventBuilder&) = delete;
			virtual ~EventBuilder();

		public:
			bool Do();

			bool IsAsync() const
			{
				return m_Async;
			}
			bool IsSkipped() const
			{
				return m_IsSkipped;
			}
			bool IsAllowed() const
			{
				return m_IsAllowed;
			}

		public:
			EventBuilder& operator=(EventBuilder&& other);
			EventBuilder& operator=(const EventBuilder&) = delete;
	};
}

template<class TEvent>
class KxEventBuilder: public Kx::EventSystem::EventBuilder
{
	public:
		KxEventBuilder(wxEvtHandler& evtHandler, std::unique_ptr<TEvent> event, std::optional<KxEventID> eventID = {})
			:EventBuilder(evtHandler, std::move(event), eventID)
		{
		}
		KxEventBuilder(wxEvtHandler& evtHandler, TEvent& event, std::optional<KxEventID> eventID = {})
			:EventBuilder(evtHandler, event, eventID)
		{
		}
		KxEventBuilder(KxEventBuilder&& other)
			:EventBuilder(other)
		{
		}

	public:
		template<class TFunctor> KxEventBuilder& On(TFunctor&& func)
		{
			std::invoke(func, static_cast<TEvent&>(*m_Event));
			return *this;
		}

	public:
		KxEventBuilder& operator=(KxEventBuilder&& other)
		{
			static_cast<EventBuilder&>(*this) = std::move(other);
			return *this;
		}
};
