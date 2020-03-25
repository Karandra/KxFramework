#pragma once
#include "Kx/EventSystem/Event.h"

namespace KxFramework
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
