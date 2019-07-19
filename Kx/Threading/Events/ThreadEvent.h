#pragma once
#include "../Common.h"

class KxThreadEvent: public wxThreadEvent
{
	public:
		KxEVENT_MEMBER(KxThreadEvent, Execute);
		KxEVENT_MEMBER(KxThreadEvent, Started);
		KxEVENT_MEMBER(KxThreadEvent, Finished);

	public:
		KxThreadEvent() = default;

	public:
		KxThreadEvent* Clone() const override
		{
			return new KxThreadEvent(*this);
		}
};
