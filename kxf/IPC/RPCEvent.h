#pragma once
#include "Common.h"
#include "Private/RPCExchange.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/EventSystem/Event.h"

namespace kxf
{
	class IRPCServer;
	class IRPCClient;
}

namespace kxf
{
	class RPCEvent: public BasicEvent
	{
		KxRTTI_DeclareIID(RPCEvent, {0x709fe4e1, 0xff7b, 0x4c2f, {0xb4, 0xce, 0xf5, 0xc3, 0x20, 0x7d, 0x55, 0xc9}});

		public:
			KxEVENT_MEMBER_STRING(RPCEvent, ServerStarted);
			KxEVENT_MEMBER_STRING(RPCEvent, ServerTerminated);

			KxEVENT_MEMBER_STRING(RPCEvent, ClientConnected);
			KxEVENT_MEMBER_STRING(RPCEvent, ClientDisconnected);

		public:
			RPCEvent() = default;
			~RPCEvent() = default;

		public:
			virtual IRPCServer* GetServer() const = 0;
			virtual IRPCClient* GetClient() const = 0;
			virtual UniversallyUniqueID GetClientID() const = 0;

			virtual bool HasResult() const = 0;
			virtual size_t GetParameterCount() const = 0;
			virtual IInputStream& RawGetParameters() = 0;
			virtual void RawSetResult(IInputStream& stream) = 0;

		public:
			template<class... Args>
			std::tuple<Args...> GetParameters()
			{
				return IPC::Private::DeserializeParameters<Args...>().AsTuple(RawGetParameters(), std::make_index_sequence<sizeof...(Args)>());
			}

			template<class TReturn>
			void SetResult(const TReturn& result)
			{
				MemoryOutputStream stream;
				IPC::Private::SetProcedureResult(stream, result);

				MemoryInputStream inputStream(stream);
				RawSetResult(inputStream);
			}
	};
}

#define KxRPC_DECLARE_EVENT(name)							extern KX_API const kxf::EventTag<kxf::RPCEvent> name

#define KxRPC_DEFINE_EVENT_AS(name, ...)					const kxf::EventTag<kxf::RPCEvent> name = (__VA_ARGS__)
#define KxRPC_DEFINE_EVENT_UUID(name, ...)					KxRPC_DEFINE_EVENT_AS(name, kxf::NativeUUID (__VA_ARGS__))
#define KxRPC_DEFINE_EVENT_STRING(name)						KxRPC_DEFINE_EVENT_AS(name, #name)
