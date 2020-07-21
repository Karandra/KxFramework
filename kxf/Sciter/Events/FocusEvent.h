#pragma once
#include "Event.h"

namespace kxf::Sciter
{
	class KX_API FocusEvent: public Event
	{
		public:
			KxEVENT_MEMBER(FocusEvent, SetFocus);
			KxEVENT_MEMBER(FocusEvent, KillFocus);
			KxEVENT_MEMBER(FocusEvent, ContainerSetFocus);
			KxEVENT_MEMBER(FocusEvent, ContainerKillFocus);
			KxEVENT_MEMBER(FocusEvent, RequestFocus);
			KxEVENT_MEMBER(FocusEvent, RequestFocusAdvanced);

		public:
			FocusEvent(Host& host)
				:Event(host)
			{
			}

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<FocusEvent>(std::move(*this));
			}
	};
}

namespace kxf::Sciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, SetFocus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, KillFocus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, ContainerSetFocus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, ContainerKillFocus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, RequestFocus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, RequestFocusAdvanced);
}
