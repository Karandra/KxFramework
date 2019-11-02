#include "KxStdAfx.h"
#include "KxFramework/KxDPIAwareness.h"
#include "KxFramework/KxSystemAPI.h"

namespace
{
	DPI_AWARENESS_CONTEXT MapAwarenessContext(KxDPIAwareness::AwarenessContext context)
	{
		using namespace KxDPIAwareness;

		switch (context)
		{
			case AwarenessContext::Unaware:
			{
				return DPI_AWARENESS_CONTEXT_UNAWARE;
			}
			case AwarenessContext::Scaled:
			{
				return DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED;
			}
			case AwarenessContext::System:
			{
				return DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
			}
			case AwarenessContext::PerMonitor:
			{
				return DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE;
			}
			case AwarenessContext::PerMonitor2:
			{
				return DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2;
			}
		};
		return nullptr;
	}
}

namespace KxDPIAwareness
{
	ThreadContextChanger::ThreadContextChanger(AwarenessContext newContext)
	{
		DPI_AWARENESS_CONTEXT context = MapAwarenessContext(newContext);
		if (context && KxSystemAPI::SetThreadDpiAwarenessContext)
		{
			m_PreviousContext = KxSystemAPI::SetThreadDpiAwarenessContext(context);
		}
	}
	ThreadContextChanger::~ThreadContextChanger()
	{
		if (KxSystemAPI::SetThreadDpiAwarenessContext)
		{
			KxSystemAPI::SetThreadDpiAwarenessContext(m_PreviousContext);
		}
	}
}
