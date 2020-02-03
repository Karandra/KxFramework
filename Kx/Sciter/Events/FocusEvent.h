#pragma once
#include "Event.h"

namespace KxSciter
{
	class KX_API FocusEvent: public Event
	{
		public:
			KxEVENT_MEMBER(FocusEvent, SetFocus);
			KxEVENT_MEMBER(FocusEvent, KillFocus);
			KxEVENT_MEMBER(FocusEvent, ContainerSetFocus);
			KxEVENT_MEMBER(FocusEvent, ContainerKillFocus);
			KxEVENT_MEMBER(FocusEvent, Request);

		protected:
			bool m_ByMouseClick = false;

		public:
			FocusEvent(Host& host)
				:Event(host)
			{
			}

		public:
			FocusEvent* Clone() const override
			{
				return new FocusEvent(*this);
			}

			bool IsByMouseClick() const
			{
				return m_ByMouseClick;
			}
			void SetByMouseClick(bool byMouseClick = true)
			{
				m_ByMouseClick = byMouseClick;
			}
	};
}

namespace KxSciter
{
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, SetFocus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, KillFocus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, ContainerSetFocus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, ContainerKillFocus);
	KxEVENT_DECLARE_ALIAS_TO_MEMBER(FocusEvent, Request);
}
