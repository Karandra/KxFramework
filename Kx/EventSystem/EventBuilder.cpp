#include "KxStdAfx.h"
#include "EventBuilder.h"
#include "BasicEvtHandler.h"
#include "Kx/Utility/Common.h"

namespace KxFramework::EventSystem
{
	EventBuilderBase::~EventBuilderBase()
	{
		// If the event wasn't sent using 'EventBuilderBase::Do', send it here
		if (!m_IsSent)
		{
			Do();
		}

		// Async events are allocated on the heap so we need to delete
		// the pointer if it's still here for some reason.
		if (m_IsAsync)
		{
			delete m_Event;
		}
	}

	EventBuilderBase& EventBuilderBase::Do()
	{
		if (m_IsAsync)
		{
			std::unique_ptr<wxEvent> event(Utility::ExchangeResetAndReturn(m_Event, nullptr));
			m_EvtHandler->DoQueueEvent(std::move(event), m_EventID);
			m_IsSent = true;
		}
		else
		{
			m_IsProcessed = m_EvtHandler->DoProcessEvent(*m_Event, m_EventID);
			m_IsSent = true;
			m_IsSkipped = m_Event->GetSkipped();
			m_IsAllowed = m_IsNotifyEvent ? static_cast<wxNotifyEvent*>(m_Event)->IsAllowed() : true;
		}
		return *this;
	}

	EventBuilderBase& EventBuilderBase::operator=(EventBuilderBase&& other)
	{
		using Utility::ExchangeAndReset;
		const EventBuilderBase null;

		ExchangeAndReset(m_EvtHandler, other.m_EvtHandler, null.m_EvtHandler);
		ExchangeAndReset(m_Event, other.m_Event, null.m_Event);
		ExchangeAndReset(m_EventID, other.m_EventID, null.m_EventID);
		ExchangeAndReset(m_IsAsync, other.m_IsAsync, null.m_IsAsync);
		ExchangeAndReset(m_IsSent, other.m_IsSent, null.m_IsSent);
		ExchangeAndReset(m_IsSkipped, other.m_IsSkipped, null.m_IsSkipped);
		ExchangeAndReset(m_IsAllowed, other.m_IsAllowed, null.m_IsAllowed);

		return *this;
	}
}
