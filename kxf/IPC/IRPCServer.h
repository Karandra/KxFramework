#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class IRPCClient;
}

namespace kxf
{
	class IRPCServer: public RTTI::Interface<IRPCServer>
	{
		KxRTTI_DeclareIID(IRPCServer, {0x6b534745, 0x90a7, 0x4e91, {0x87, 0xab, 0x44, 0x8b, 0x99, 0xcc, 0x66, 0xb0}});

		public:
			virtual bool StartServer() = 0;
			virtual void TerminateServer() = 0;
	};
}
