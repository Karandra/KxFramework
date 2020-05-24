#pragma once
#include "Event.h"
#include <optional>

namespace kxf
{
	class BasicEvtHandler;
}

namespace kxf::EventSystem
{
	class KX_API EventBuilderBase
	{
		protected:
			BasicEvtHandler* m_EvtHandler = nullptr;
			wxEvent* m_Event = nullptr;
			std::optional<EventID> m_EventID;

			bool m_IsNotifyEvent = false;
			bool m_IsAsync = false;
			bool m_IsSent = false;
			bool m_IsSkipped = false;
			bool m_IsAllowed = true;
			bool m_IsProcessed = false;

		private:
			EventBuilderBase() = default;

		public:
			EventBuilderBase(BasicEvtHandler& evtHandler, std::unique_ptr<wxEvent> event, std::optional<EventID> eventID = {})
				:m_EvtHandler(&evtHandler), m_Event(event.release()), m_EventID(eventID), m_IsAsync(true)
			{
			}
			EventBuilderBase(BasicEvtHandler& evtHandler, wxEvent& event, std::optional<EventID> eventID = {})
				:m_EvtHandler(&evtHandler), m_Event(&event), m_EventID(eventID), m_IsAsync(false)
			{
			}
			EventBuilderBase(EventBuilderBase&& other)
			{
				*this = std::move(other);
			}
			EventBuilderBase(const EventBuilderBase&) = delete;
			virtual ~EventBuilderBase();

		public:
			EventBuilderBase& Do();

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
			EventBuilderBase& operator=(EventBuilderBase&& other);
			EventBuilderBase& operator=(const EventBuilderBase&) = delete;
	};
}

namespace kxf::EventSystem
{
	template<class TEvent>
	class EventBuilder: public EventBuilderBase
	{
		private:
			void TestEventClass()
			{
				m_IsNotifyEvent = std::is_base_of_v<wxNotifyEvent, TEvent>;
			}

		public:
			EventBuilder(BasicEvtHandler& evtHandler, std::unique_ptr<TEvent> event, std::optional<EventID> eventID = {})
				:EventBuilderBase(evtHandler, std::move(event), eventID)
			{
				TestEventClass();
			}
			EventBuilder(BasicEvtHandler& evtHandler, TEvent& event, std::optional<EventID> eventID = {})
				:EventBuilderBase(evtHandler, event, eventID)
			{
				TestEventClass();
			}
			EventBuilder(EventBuilder&& other)
				:EventBuilderBase(std::move(other))
			{
			}

		public:
			template<class TFunctor>
			EventBuilder& On(TFunctor&& func)
			{
				std::invoke(func, static_cast<TEvent&>(*m_Event));
				return *this;
			}
			
			EventBuilder& Do()
			{
				EventBuilderBase::Do();
				return *this;
			}

		public:
			EventBuilder& operator=(EventBuilder&& other)
			{
				static_cast<EventBuilderBase&>(*this) = std::move(other);
				return *this;
			}
	};
}
