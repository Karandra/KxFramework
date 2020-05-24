#include "stdafx.h"
#include "JoinableThread.h"

namespace kxf
{
	wxThread::ExitCode JoinableThread::Entry()
	{
		QueueEvent(ThreadEvent::EvtStarted);

		ThreadEvent event;
		ProcessEvent(event, ThreadEvent::EvtExecute);

		QueueEvent(ThreadEvent::EvtFinished);

		return event.GetExitCode();
	}
}
