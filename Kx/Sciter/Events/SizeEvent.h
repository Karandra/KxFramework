#pragma once
#include "Event.h"

namespace KxFramework::Sciter
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
			SizeEvent* Clone() const override
			{
				return new SizeEvent(*this);
			}
	};
}

namespace KxFramework::Sciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(SizeEvent, Size);
}
