#pragma once
#include "Event.h"

namespace kxf::Sciter
{
	class KX_API SizeEvent: public Event
	{
		public:
			KxEVENT_MEMBER(SizeEvent, Size);

		public:
			SizeEvent(Host& host)
				:Event(host)
			{
			}

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<SizeEvent>(std::move(*this));
			}
	};
}

namespace kxf::Sciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(SizeEvent, Size);
}
