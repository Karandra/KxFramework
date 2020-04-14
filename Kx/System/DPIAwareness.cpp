#include "KxStdAfx.h"
#include "DPIAwareness.h"
#include "NativeAPI.h"

namespace
{
	DPI_AWARENESS_CONTEXT MapAwarenessContext(KxFramework::DPIAwarenessContext context)
	{
		using namespace KxFramework;

		switch (context)
		{
			case DPIAwarenessContext::Unaware:
			{
				return DPI_AWARENESS_CONTEXT_UNAWARE;
			}
			case DPIAwarenessContext::Scaled:
			{
				return DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED;
			}
			case DPIAwarenessContext::System:
			{
				return DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
			}
			case DPIAwarenessContext::PerMonitor:
			{
				return DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE;
			}
			case DPIAwarenessContext::PerMonitor2:
			{
				return DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2;
			}
		};
		return nullptr;
	}
}

namespace KxFramework
{
	ChangeDPIAwareness::ChangeDPIAwareness(DPIAwarenessContext newContext)
	{
		if (NativeAPI::User32::SetThreadDpiAwarenessContext)
		{
			if (DPI_AWARENESS_CONTEXT context = MapAwarenessContext(newContext))
			{
				m_PreviousContext = NativeAPI::User32::SetThreadDpiAwarenessContext(context);
			}
		}
	}
	ChangeDPIAwareness::~ChangeDPIAwareness()
	{
		if (NativeAPI::User32::SetThreadDpiAwarenessContext && m_PreviousContext)
		{
			NativeAPI::User32::SetThreadDpiAwarenessContext(m_PreviousContext);
		}
	}
}
