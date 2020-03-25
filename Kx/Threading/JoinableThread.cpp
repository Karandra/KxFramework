#include "KxStdAfx.h"
#include "JoinableThread.h"

namespace KxFramework
{
	wxThread::ExitCode JoinableThread::Entry()
	{
		QueueEvent(KxThreadEvent::EvtStarted);

		KxThreadEvent event;
		ProcessEvent(event, KxThreadEvent::EvtExecute);

		QueueEvent(KxThreadEvent::EvtFinished);

		return event.GetExitCode();
	}
}
