#include "stdafx.h"
#include "CoreApplication.h"
#include "kxf/EventSystem/IEventExecutor.h"
#include "kxf/EventSystem/IdleEvent.h"

namespace kxf
{
	bool CoreApplication::DispatchIdle()
	{
		// Synthesize an idle event and check if more of them are needed
		IdleEvent& event = BuildProcessEvent(IdleEvent::EvtIdle).SetSourceToSelf().Execute().GetEvent();

		// Flush the logged messages if any (do this after processing the events which could have logged new messages)
		wxLog::FlushActive();

		// Garbage collect all objects previously scheduled for destruction
		FinalizeScheduledForDestruction();

		return event.IsMoreRequested();
	}

	void CoreApplication::ExecuteEventHandler(Event& event, IEventExecutor& executor, EvtHandler& evtHandler)
	{
		executor.Execute(evtHandler, event);
	}

	size_t CoreApplication::EnumCommandLineArgs(std::function<bool(String)> func) const
	{
		size_t count = 0;
		for (size_t i = 0; i < static_cast<size_t>(m_NativeApp.argc); i++)
		{
			count++;
			if (!std::invoke(func, m_NativeApp.argv[i]))
			{
				break;
			}
		}
		return count;
	}
}
