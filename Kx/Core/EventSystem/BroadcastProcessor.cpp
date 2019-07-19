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

		m_Processor.EnumRecieveres([this, &event](wxEvtHandler& evtHandler)
		{
			evtHandler.ProcessEventLocally(event);
			return true;
		});
		return true;
	}
}

namespace Kx::EventSystem
{
	bool BroadcastRecieverHandler::UnbindAll(KxEventID eventID)
	{
		// See 'wxEvtHandler::DoUnbind' (wxWidgets/src/event.cpp) for details
		if (m_dynamicEvents)
		{
			size_t deletedCount = 0;
			for (auto it = m_dynamicEvents->rbegin(); it != m_dynamicEvents->rend(); ++it)
			{
				wxDynamicEventTableEntry* entry = *it;
				if (entry && entry->m_eventType == eventID)
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
		m_EvtHandler.SetNextHandler(&evtHandler);
		evtHandler.SetPreviousHandler(&m_EvtHandler);

		return true;
	}
	return false;
}
bool KxBroadcastProcessor::RemoveReciever(KxBroadcastReciever& reciever)
{
	// Check if this handler is part of our chain
	bool removed = false;
	EnumRecieveres([&reciever, &removed](wxEvtHandler& chainItem)
	{
		// Unlink it
		if (&chainItem == &reciever.GetEvtHandler())
		{
			chainItem.Unlink();
			removed = true;
		}
		return !removed;
	});
	return removed;
}
