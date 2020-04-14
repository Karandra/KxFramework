#pragma once
#include "Common.h"

namespace KxFramework
{
	enum class DPIAwarenessContext
	{
		Unknown = 0,

		Unaware,
		Scaled,
		System,
		PerMonitor,
		PerMonitor2,
	};
}

namespace KxFramework
{
	class ChangeDPIAwareness final
	{
		private:
			void* m_PreviousContext = nullptr;

		public:
			ChangeDPIAwareness(DPIAwarenessContext newContext);
			~ChangeDPIAwareness();
	};
}
