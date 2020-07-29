#pragma once
#include "Common.h"
#include "IEvent.h"
#include "kxf/Utility/Common.h"

namespace kxf::EventSystem
{
	enum class EventPublicState: uint32_t
	{
		None = 0,

		Skipped = 1 << 0,
		Allowed = 1 << 1
	};
	enum class EventPrivateState: uint32_t
	{
		None = 0,

		// Set after a call to 'OnStartProcess'.
		Started = 1 << 0,

		// Set as soon as 'WasQueueed' is called for the first time when event is going to be queued from 'DoProcessEvent'.
		Queueed = 1 << 1,

		// Set after 'DoProcessEvent' was called at least once for this event.
		ProcessedOnce = 1 << 2,

		// Can be set to indicate that the event will be passed to another handler if it's not processed in this one.
		WillBeProcessedAgain = 1 << 3
	};
}
namespace kxf
{
	KxDeclareFlagSet(EventSystem::EventPublicState);
	KxDeclareFlagSet(EventSystem::EventPrivateState);
}

namespace kxf
{
	class KX_API BasicEvent: public RTTI::ImplementInterface<BasicEvent, IEvent>
	{
		private:
			using EventPublicState = EventSystem::EventPublicState;
			using EventPrivateState = EventSystem::EventPrivateState;

		private:
			EventID m_EventID;
			IEvtHandler* m_EventSource = nullptr;
			UniversallyUniqueID m_UniqueID;
			TimeSpan m_Timestamp;

			FlagSet<EventPublicState> m_PublicState = EventPublicState::Allowed;
			mutable FlagSet<EventPrivateState> m_PrivateState;

		private:
			bool TestAndSetPrivateState(EventPrivateState flag) const
			{
				if (!m_PrivateState.Contains(flag))
				{
					m_PrivateState.Add(flag);
					return false;
				}
				return true;
			}

			bool WasQueueed() const override
			{
				return TestAndSetPrivateState(EventPrivateState::Queueed);
			}
			bool WasProcessed() const override
			{
				return TestAndSetPrivateState(EventPrivateState::ProcessedOnce);
			}
			bool WillBeProcessedAgain() const override
			{
				return TestAndSetPrivateState(EventPrivateState::WillBeProcessedAgain);
			}

			void OnStartProcess(const EventID& eventID, const UniversallyUniqueID& uuid) override
			{
				if (!m_PrivateState.Contains(EventPrivateState::Started))
				{
					m_PrivateState.Add(EventPrivateState::Started);

					m_EventID = eventID;
					m_UniqueID = std::move(uuid);
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

			IEvtHandler* GetEventSource() const override
			{
				return m_EventSource;
			}
			void SetEventSource(IEvtHandler* evtHandler) override
			{
				m_EventSource = evtHandler;
			}
			
			bool IsSkipped() const override
			{
				return m_PublicState.Contains(EventPublicState::Skipped);
			}
			void Skip(bool skip = true) override
			{
				m_PublicState.Mod(EventPublicState::Skipped, skip);
			}

			bool IsAllowed() const override
			{
				return m_PublicState.Contains(EventPublicState::Allowed);
			}
			void Allow(bool allow = true) override
			{
				m_PublicState.Mod(EventPublicState::Allowed, allow);
			}

		public:
			BasicEvent& operator=(const BasicEvent&) noexcept = default;
			BasicEvent& operator=(BasicEvent&& other) noexcept
			{
				m_EventID = std::move(other.m_EventID);
				m_EventSource = Utility::ExchangeResetAndReturn(other.m_EventSource, nullptr);
				m_UniqueID = std::move(other.m_UniqueID);
				m_Timestamp = std::move(other.m_Timestamp);

				m_PublicState = Utility::ExchangeResetAndReturn(other.m_PublicState, EventPublicState::Allowed);
				m_PrivateState = Utility::ExchangeResetAndReturn(other.m_PrivateState, EventPrivateState::None);

				return *this;
			}
	};
}
