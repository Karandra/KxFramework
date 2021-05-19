#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/EventSystem/Event.h"

namespace kxf
{
	class IRPCServer;
	class IRPCClient;

	class IInputStream;
	class IOutputStream;
}

namespace kxf
{
	class IRPCEvent: public RTTI::ExtendInterface<IRPCEvent, BasicEvent>
	{
		KxRTTI_DeclareIID(IRPCEvent, {0x709fe4e1, 0xff7b, 0x4c2f, {0xb4, 0xce, 0xf5, 0xc3, 0x20, 0x7d, 0x55, 0xc9}});

		public:
			KxEVENT_MEMBER(IRPCEvent, ServerStarted);
			KxEVENT_MEMBER(IRPCEvent, ServerTerminated);

			KxEVENT_MEMBER(IRPCEvent, ClientStarted);
			KxEVENT_MEMBER(IRPCEvent, ClientTerminated);

			KxEVENT_MEMBER(IRPCEvent, ProcedureCall);

		public:
			IRPCEvent() = default;
			~IRPCEvent() = default;

		public:
			virtual IRPCServer* GetServer() const = 0;
			virtual IRPCClient* GetClient() const = 0;

			virtual IInputStream& GetProcedureResult() = 0;
			virtual void SetProcedureResult(IInputStream& stream) = 0;

			virtual IInputStream& GetProcedureParameters() = 0;
			virtual void SetProcedureParameters(IInputStream& stream) = 0;
	};
}
