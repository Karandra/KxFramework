#pragma once
#include "KxFramework/KxFramework.h"

namespace KxDPIAwareness
{
	enum class AwarenessContext
	{
		Unknown = 0,

		Unaware,
		Scaled,
		System,
		PerMonitor,
		PerMonitor2,
	};

	class ThreadContextChanger final
	{
		private:
			void* m_PreviousContext = nullptr;

		public:
			ThreadContextChanger(AwarenessContext newContext);
			~ThreadContextChanger();
	};
}
