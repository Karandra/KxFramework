#include "stdafx.h"
#include "EventBuilder.h"
#include "IEvtHandler.h"
#include "kxf/Utility/Common.h"

namespace kxf::EventSystem
{
	void EventBuilderBase::Destroy() noexcept
	{
		// Async events are allocated on the heap so we need to delete the pointer if it's still here for some reason.
		if (m_IsAsync)
		{
			delete m_Event;
		}
	}
	void EventBuilderBase::Move(EventBuilderBase&& other) noexcept
	{
		Destroy();

		EventBuilderBase null;
		Utility::ExchangeAndReset(m_EvtHandler, other.m_EvtHandler, null.m_EvtHandler);
		Utility::ExchangeAndReset(m_Event, other.m_Event, null.m_Event);
		m_EventID = std::move(other.m_EventID);
		Utility::ExchangeAndReset(m_IsAsync, other.m_IsAsync, null.m_IsAsync);
		Utility::ExchangeAndReset(m_IsSent, other.m_IsSent, null.m_IsSent);
		Utility::ExchangeAndReset(m_IsSkipped, other.m_IsSkipped, null.m_IsSkipped);
		Utility::ExchangeAndReset(m_IsAllowed, other.m_IsAllowed, null.m_IsAllowed);
	}

	void EventBuilderBase::SendEvent(bool locally, bool safely, UniversallyUniqueID uuid)
	{
		if (!m_IsSent)
		{
			if (m_IsAsync)
			{
				m_IsSent = true;
				m_IsSkipped = false;
				m_IsAllowed = true;

				std::unique_ptr<IEvent> event(Utility::ExchangeResetAndReturn(m_Event, nullptr));
				m_EvtHandler->QueueEvent(std::move(event), m_EventID, std::move(uuid));
			}
			else
			{
				m_IsSent = true;
				if (locally)
				{
					m_IsProcessed = m_EvtHandler->ProcessEventLocally(*m_Event, m_EventID);
				}
				else if (safely)
				{
					m_IsProcessed = m_EvtHandler->ProcessEventSafely(*m_Event, m_EventID);
				}
				else
				{
					m_IsProcessed = m_EvtHandler->ProcessEvent(*m_Event, m_EventID);
				}

				m_IsSkipped = m_Event->IsSkipped();
				m_IsAllowed = m_Event->IsAllowed();
			}
		}
	}
}
