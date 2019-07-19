#include "KxStdAfx.h"
#include "Thread.h"

wxThread::ExitCode KxThread::Entry()
{
	QueueEvent(KxThreadEvent::EvtStarted);
	ProcessEvent(KxThreadEvent::EvtExecute);
	QueueEvent(KxThreadEvent::EvtFinished);
}
