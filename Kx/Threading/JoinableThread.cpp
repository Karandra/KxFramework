#include "KxStdAfx.h"
#include "JoinableThread.h"

namespace KxFramework
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
