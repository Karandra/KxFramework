#pragma once
#include "Common.h"

namespace kxf
{
	class IEvent;
	class EvtHandler;
}

namespace kxf
{
	class KX_API IEventExecutor
	{
		public:
			virtual ~IEventExecutor() = default;

		public:
			virtual void Execute(EvtHandler& evtHandler, IEvent& event) = 0;
			virtual bool IsSameAs(const IEventExecutor& other) const noexcept = 0;
			virtual EvtHandler* GetTargetHandler() noexcept = 0;
	};
}
