#include "stdafx.h"
#include "EventBuilder.h"
#include "IEvtHandler.h"
#include "kxf/Utility/Common.h"

namespace kxf::EventSystem
{
	void EventBuilderBase::Move(EventBuilderBase&& other) noexcept
	{
		EventBuilderBase null;
		Utility::ExchangeAndReset(m_EvtHandler, other.m_EvtHandler, null.m_EvtHandler);
		m_Event = std::move(other.m_Event);
		m_EventID = std::move(other.m_EventID);
		Utility::ExchangeAndReset(m_IsSent, other.m_IsSent, null.m_IsSent);
		Utility::ExchangeAndReset(m_IsSkipped, other.m_IsSkipped, null.m_IsSkipped);
		Utility::ExchangeAndReset(m_IsAllowed, other.m_IsAllowed, null.m_IsAllowed);
	}
	void EventBuilderBase::SendEvent(bool locally, bool safely, UniversallyUniqueID uuid)
	{
		if (!m_IsSent)
		{
			if (m_Event.IsOwned())
			{
				m_IsSent = true;
				m_IsSkipped = false;
				m_IsAllowed = true;

				m_EvtHandler->QueueEvent(std::move(m_Event).GetUnique(), m_EventID, std::move(uuid));
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
