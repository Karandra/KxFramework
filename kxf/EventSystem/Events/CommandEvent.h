#pragma once
#include "kxf/EventSystem/Event.h"

namespace kxf
{
	class CommandEvent: public wxCommandEvent
	{
		public:
			CommandEvent() = default;

		public:
			CommandEvent* Clone() const override
			{
				return new CommandEvent(*this);
			}
	};
}
