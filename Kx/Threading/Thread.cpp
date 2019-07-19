#include "KxStdAfx.h"
#include "Thread.h"

wxThread::ExitCode KxThread::Entry()
{
	QueueEvent(KxThreadEvent::EvtStarted);

	KxThreadEvent event;
	ProcessEvent(event, KxThreadEvent::EvtExecute);

	QueueEvent(KxThreadEvent::EvtFinished);

	return event.GetExitCode();
}
