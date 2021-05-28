#pragma once
#include "Common.h"
#include "RPCEvent.h"
#include "Private/RPCExchange.h"

namespace kxf
{
	class IRPCServer;
	class IEvtHandler;
	class IThreadPool;
}

namespace kxf
{
	class IRPCClient: public RTTI::Interface<IRPCClient>
	{
		KxRTTI_DeclareIID(IRPCClient, {0x57b31a53, 0x4fbd, 0x454f, {0x93, 0xb, 0xd0, 0x5b, 0xfb, 0x74, 0xe, 0x57}});

		public:
			virtual bool IsConnectedToServer() const = 0;
			virtual bool ConnectToServer(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler, const UniversallyUniqueID& clientID = {}, std::shared_ptr<IThreadPool> threadPool = {}, FlagSet<RPCExchangeFlag> flags = {}) = 0;
			virtual void DisconnectFromServer() = 0;
			virtual UniversallyUniqueID GetSessionID() const = 0;
			virtual UniversallyUniqueID GetClientID() const = 0;

			virtual MemoryInputStream RawInvokeProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult) = 0;

		public:
			template<class TReturn = void, class... Args>
			TReturn InvokeProcedure(const EventID& procedureID, Args&&... arg)
			{
				return IPC::Private::InvokeProcedure<TReturn>([&](MemoryOutputStream& parametersStream, size_t parametersCount, bool hasResult)
				{
					MemoryInputStream parametersInputStream(parametersStream.DetachStreamBuffer());
					return RawInvokeProcedure(procedureID, parametersInputStream, parametersCount, hasResult);
				}, std::forward<Args>(arg)...);
			}
	};
}
