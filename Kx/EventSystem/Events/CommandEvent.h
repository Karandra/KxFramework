#pragma once
#include "Kx/EventSystem/Event.h"

class KxCommandEvent: public wxCommandEvent
{
	public:
		KxCommandEvent() = default;

	public:
		KxCommandEvent* Clone() const override
		{
			return new KxCommandEvent(*this);
		}
};
