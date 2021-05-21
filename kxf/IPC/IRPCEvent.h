#pragma once
#include "Common.h"
#include "Private/RPCExchange.h"
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

			KxEVENT_MEMBER(IRPCEvent, ClientConnected);
			KxEVENT_MEMBER(IRPCEvent, ClientDisconnected);

		public:
			IRPCEvent() = default;
			~IRPCEvent() = default;

		public:
			virtual IRPCServer* GetServer() const = 0;
			virtual IRPCClient* GetClient() const = 0;

			virtual IInputStream& RawGetProcedureResult() = 0;
			virtual void RawSetProcedureResult(IInputStream& stream) = 0;

			virtual IInputStream& RawGetProcedureParameters() = 0;
			virtual void RawSetProcedureParameters(IInputStream& stream) = 0;

		public:
			template<class... Args>
			std::tuple<Args...> GetProcedureParameters()
			{
				return IPC::Private::DeserializeParameters<Args...>().AsTuple(RawGetProcedureParameters(), std::make_index_sequence<sizeof...(Args)>());
			}

			template<class TReturn>
			void SetProcedureResult(const TReturn& result)
			{
				MemoryOutputStream stream;
				IPC::Private::SetProcedureResult(stream, result);

				MemoryInputStream inputStream(stream);
				RawSetProcedureResult(inputStream);
			}
	};
}
