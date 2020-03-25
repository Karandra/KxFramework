#pragma once
#include "Kx/EventSystem/Event.h"

namespace KxFramework
{
	class NotifyEvent: public wxNotifyEvent
	{
		public:
			NotifyEvent() = default;

		public:
			NotifyEvent* Clone() const override
			{
				return new NotifyEvent(*this);
			}
	};
}
