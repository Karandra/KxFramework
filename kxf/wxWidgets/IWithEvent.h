#pragma once
#include "Common.h"

namespace kxf::wxWidgets
{
	class KX_API IWithEvent: public RTTI::Interface<IWithEvent>
	{
		KxDeclareIID(IWithEvent, {0x8154331b, 0x997b, 0x4a28, {0xa8, 0xaf, 0x95, 0xb0, 0x57, 0x12, 0x1d, 0x6f}});

		public:
			virtual ~IWithEvent() = default;

		public:
			virtual wxEvent& GetEvent() = 0;
	};
}
