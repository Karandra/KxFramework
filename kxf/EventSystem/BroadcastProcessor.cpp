#include "stdafx.h"
#include "BroadcastProcessor.h"

namespace kxf::EventSystem
{
	bool BroadcastProcessorHandler::TryBefore(IEvent& event)
	{
		m_Processor.EnumRecieveres([this, &event](EvtHandler& evtHandler)
		{
			evtHandler.ProcessEventLocally(event, event.GetEventID());
			return true;
		});
		return true;
	}
}

namespace kxf::EventSystem
{
	bool BroadcastRecieverHandler::TryBefore(IEvent& event)
	{
		TryHereOnly(event);
		return true;
	}
}

namespace kxf
{
	bool EventBroadcastProcessor::AddReciever(EventBroadcastReciever& reciever)
	{
		return m_Stack.Push(reciever.GetEvtHandler());
	}
	bool EventBroadcastProcessor::RemoveReciever(EventBroadcastReciever& reciever)
	{
		return m_Stack.Remove(reciever.GetEvtHandler());
	}

	bool EventBroadcastReciever::PreProcessEvent(IEvent& event)
	{
		StopPropagation(event);
		return m_Processor.PreProcessEvent(event);
	}
	void EventBroadcastReciever::PostProcessEvent(IEvent& event)
	{
		m_Processor.PostProcessEvent(event);
	}
	void EventBroadcastReciever::FinalPostProcessEvent(IEvent& event)
	{
		event.Skip();
		StopPropagation(event);
	}
}
