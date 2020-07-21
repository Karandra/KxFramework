#pragma once
#include "Common.h"
#include "IEvent.h"
#include "kxf/Utility/Common.h"

namespace kxf
{
	class KX_API BasicEvent: public RTTI::ImplementInterface<BasicEvent, IEvent>
	{
		private:
			EventID m_EventID;
			EvtHandler* m_EventSource = nullptr;
			UniversallyUniqueID m_UniqueID;
			TimeSpan m_Timestamp;

			bool m_IsAllowed = true;
			bool m_IsSkipped = false;

			// Initially false but becomes true as soon as 'WasProcessed' is called for the first time,
			// as this is done only by 'DoProcessEvent' it explains the variable name: it becomes true
			// after 'DoProcessEvent' was called at least once for this event.
			mutable bool m_WasProcessed = false;

			// This one is initially false too, but can be set to true to indicate that
			// the event will be passed to another handler if it's not processed in this one.
			mutable bool m_WillBeProcessedAgain = false;

		private:
			bool WasProcessed() const override
			{
				if (m_WasProcessed)
				{
					return true;
				}

				m_WasProcessed = true;
				return false;
			}
			bool WillBeProcessedAgain() const override
			{
				if (m_WillBeProcessedAgain)
				{
					m_WillBeProcessedAgain = false;
					return true;
				}
				return false;
			}

			void OnStartProcess(const EventID& eventID, const UniversallyUniqueID& uuid) override
			{
				if (eventID && !m_EventID)
				{
					m_EventID = eventID;
				}
				if (!m_UniqueID)
				{
					m_UniqueID = std::move(uuid);
				}
				if (!m_Timestamp.IsPositive())
				{
					m_Timestamp = TimeSpan::Now(SteadyClock());
				}
			}

		public:
			BasicEvent() = default;
			BasicEvent(const BasicEvent&) noexcept = default;
			BasicEvent(BasicEvent&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<BasicEvent>(std::move(*this));
			}

			EventID GetEventID() const override
			{
				return m_EventID;
			}
			TimeSpan GetTimestamp() const override
			{
				return m_Timestamp;
			}
			UniversallyUniqueID GetUniqueID() const override
			{
				return m_UniqueID;
			}
			FlagSet<EventCategory> GetEventCategory() const override
			{
				return EventCategory::None;
			}

			EvtHandler* GetEventSource() const override
			{
				return m_EventSource;
			}
			void SetEventSource(EvtHandler* evtHandler) override
			{
				m_EventSource = evtHandler;
			}
			
			bool IsSkipped() const override
			{
				return m_IsSkipped;
			}
			void Skip(bool skip = true) override
			{
				m_IsSkipped = skip;
			}

			bool IsAllowed() const override
			{
				return m_IsAllowed;
			}
			void Allow(bool allow = true) override
			{
				m_IsAllowed = allow;
			}

		public:
			BasicEvent& operator=(const BasicEvent&) noexcept = default;
			BasicEvent& operator=(BasicEvent&& other) noexcept
			{
				m_EventID = std::move(other.m_EventID);
				m_EventSource = Utility::ExchangeResetAndReturn(other.m_EventSource, nullptr);
				m_IsAllowed = Utility::ExchangeResetAndReturn(other.m_IsAllowed, true);
				m_IsSkipped = Utility::ExchangeResetAndReturn(other.m_IsSkipped, false);
				m_WasProcessed = Utility::ExchangeResetAndReturn(other.m_WasProcessed, false);
				m_WillBeProcessedAgain = Utility::ExchangeResetAndReturn(other.m_WillBeProcessedAgain, false);

				return *this;
			}
	};
}
