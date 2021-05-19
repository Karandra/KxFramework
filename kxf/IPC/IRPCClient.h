#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class IRPCServer;
}

namespace kxf
{
	class IRPCClient: public RTTI::Interface<IRPCClient>
	{
		KxRTTI_DeclareIID(IRPCClient, {0x57b31a53, 0x4fbd, 0x454f, {0x93, 0xb, 0xd0, 0x5b, 0xfb, 0x74, 0xe, 0x57}});

		public:
			virtual bool ConnectToServer() = 0;
	};
}
