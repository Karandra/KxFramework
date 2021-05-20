#pragma once
#include "Common.h"
#include "IRPCEvent.h"
#include "Private/RPCExchange.h"

namespace kxf
{
	class IRPCClient;
	class IEvtHandler;
}

namespace kxf
{
	class IRPCServer: public RTTI::Interface<IRPCServer>
	{
		KxRTTI_DeclareIID(IRPCServer, {0x6b534745, 0x90a7, 0x4e91, {0x87, 0xab, 0x44, 0x8b, 0x99, 0xcc, 0x66, 0xb0}});

		public:
			virtual bool IsServerRunning() const = 0;
			virtual bool StartServer(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler) = 0;
			virtual void TerminateServer() = 0;

			virtual void RawBroadcastProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount) = 0;

		public:
			template<class... Args>
			void BroadcastProcedure(const EventID& procedureID, Args&&... arg)
			{
				return IPC::Private::InvokeProcedure<void>([&](MemoryOutputStream& parametersStream, size_t parametersCount, bool hasResult)
				{
					MemoryInputStream parametersInputStream(parametersStream);
					RawBroadcastProcedure(procedureID, parametersInputStream, parametersCount);
				}, procedureID, std::forward<Args>(arg)...);
			}
	};
}
