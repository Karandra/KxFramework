#include "KxStdAfx.h"
#include "BroadcastProcessor.h"

namespace Kx::EventSystem
{
	bool BroadcastProcessorHandler::TryBefore(wxEvent& event)
	{
		if (event.GetEventType() == KxIndirectCallEvent::EvtIndirectCall)
		{
			return false;
		}

		m_Processor.EnumRecieveres(m_Processor.GetRecieversOrder(), [this, &event](wxEvtHandler& evtHandler)
		{
			evtHandler.ProcessEventLocally(event);
			return true;
		});
		return true;
	}

	BroadcastProcessorHandler::BroadcastProcessorHandler(KxBroadcastProcessor& processor)
		:m_Processor(processor)
	{
	}
}

namespace Kx::EventSystem
{
	bool BroadcastRecieverHandler::TryBefore(wxEvent& event)
	{
		TryHereOnly(event);
		return true;
	}
	bool BroadcastRecieverHandler::UnbindAll(std::optional<KxEventID> eventID)
	{
		// See 'wxEvtHandler::DoUnbind' (wxWidgets/src/event.cpp) for details
		if (m_dynamicEvents)
		{
			size_t deletedCount = 0;
			for (auto it = m_dynamicEvents->rbegin(); it != m_dynamicEvents->rend(); ++it)
			{
				wxDynamicEventTableEntry* entry = *it;
				if (entry && (!eventID || entry->m_eventType == eventID))
				{
					*it = nullptr;

					delete entry->m_callbackUserData;
					delete entry;

					deletedCount++;
				}
			}
			return deletedCount != 0;
		}
		return false;
	}
}

bool KxBroadcastProcessor::AddReciever(KxBroadcastReciever& reciever)
{
	wxEvtHandler& evtHandler = reciever.GetEvtHandler();

	// New handler can't be part of another chain
	if (evtHandler.IsUnlinked())
	{
		m_LastEvtHandler->SetNextHandler(&evtHandler);
		evtHandler.SetPreviousHandler(m_LastEvtHandler);
		m_LastEvtHandler = &evtHandler;

		return true;
	}
	return false;
}
bool KxBroadcastProcessor::RemoveReciever(KxBroadcastReciever& reciever)
{
	wxEvtHandler& evtHandler = reciever.GetEvtHandler();

	// Check if this handler is part of a chain at all
	if (!evtHandler.IsUnlinked())
	{
		// Short circuit for last handler
		if (&evtHandler == m_LastEvtHandler)
		{
			wxEvtHandler* previous = m_LastEvtHandler->GetPreviousHandler();
			m_LastEvtHandler->Unlink();
			m_LastEvtHandler = previous;
		}

		// Is it part of our chain?
		wxEvtHandler* unlinked = EnumRecieveres(m_Order, [&evtHandler](wxEvtHandler& chainItem)
		{
			// Unlink it
			if (&chainItem == &evtHandler)
			{
				chainItem.Unlink();
				return false;
			}
			return true;
		});
		return unlinked != nullptr;
	}
	return false;
}

bool KxBroadcastReciever::PreProcessEvent(wxEvent& event)
{
	event.StopPropagation();
	return m_Processor.PreProcessEvent(event);
}
void KxBroadcastReciever::PostProcessEvent(wxEvent& event)
{
	m_Processor.PostProcessEvent(event);
}
void KxBroadcastReciever::FinalPostProcessEvent(wxEvent& event)
{
	event.Skip();
	event.StopPropagation();
}
