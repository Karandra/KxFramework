#pragma once
#include "KxFramework/KxFramework.h"

class KxISerializer
{
	public:
		virtual bool Serialize(wxOutputStream& stream) const = 0;
		virtual bool DeSerialize(wxInputStream& stream) = 0;
};
