#pragma once
#include "../Event.h"

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
