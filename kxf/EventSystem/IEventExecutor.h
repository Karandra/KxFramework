#pragma once
#include "Common.h"

namespace kxf
{
	class Event;
	class EvtHandler;

	class KX_API IEventExecutor
	{
		public:
			virtual ~IEventExecutor() = default;

		public:
			virtual void Execute(EvtHandler& evtHandler, Event& event) = 0;
			virtual bool IsSameAs(const IEventExecutor& other) const noexcept = 0;
			virtual EvtHandler* GetTargetHandler() noexcept = 0;
	};
}
