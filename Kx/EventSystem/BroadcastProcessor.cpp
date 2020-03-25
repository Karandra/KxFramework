#include "KxStdAfx.h"
#include "BroadcastProcessor.h"

namespace KxFramework::EventSystem
{
	bool BroadcastProcessorHandler::TryBefore(wxEvent& event)
	{
		if (event.GetEventType() == IIndirectCallEvent::EvtIndirectCall)
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

	BroadcastProcessorHandler::BroadcastProcessorHandler(BroadcastProcessor& processor)
		:m_Processor(processor)
	{
	}
}

namespace KxFramework::EventSystem
{
	bool BroadcastRecieverHandler::TryBefore(wxEvent& event)
	{
		TryHereOnly(event);
		return true;
	}
	bool BroadcastRecieverHandler::UnbindAll(std::optional<EventID> eventID)
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

namespace KxFramework
{
	bool BroadcastProcessor::AddReciever(BroadcastReciever& reciever)
	{
		return m_Stack.Push(reciever.GetEvtHandler());
	}
	bool BroadcastProcessor::RemoveReciever(BroadcastReciever& reciever)
	{
		return m_Stack.Remove(reciever.GetEvtHandler());
	}

	bool BroadcastReciever::PreProcessEvent(wxEvent& event)
	{
		event.StopPropagation();
		return m_Processor.PreProcessEvent(event);
	}
	void BroadcastReciever::PostProcessEvent(wxEvent& event)
	{
		m_Processor.PostProcessEvent(event);
	}
	void BroadcastReciever::FinalPostProcessEvent(wxEvent& event)
	{
		event.Skip();
		event.StopPropagation();
	}
}
