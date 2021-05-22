#pragma once
#include "IRPCClient.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "Private/DefaultRPCExchanger.h"

namespace kxf
{
	class DefaultRPCClient: public RTTI::DynamicImplementation<DefaultRPCClient, IRPCClient>, public DefaultRPCExchanger
	{
		private:
			EvtHandler m_ServiceEvtHandler;
			IEvtHandler* m_UserEvtHandler = nullptr;

			uint32_t m_ServerPID = 0;
			void* m_ServerHandle = nullptr;

		private:
			void Notify(const EventID& eventID);
			void NotifyServer(const EventID& eventID);

			bool DoConnectToServer(KernelObjectNamespace ns, bool notify = false);
			void DoDisconnectFromServer(bool notify = false);

		protected:
			// Private::DefaultRPCExchanger
			void OnDataRecieved(IInputStream& stream) override;

		public:
			DefaultRPCClient();
			~DefaultRPCClient();

		public:
			// IRPCClient
			bool IsConnectedToServer() const override;
			bool ConnectToServer(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler, KernelObjectNamespace ns = KernelObjectNamespace::Local) override;
			void DisconnectFromServer() override;

			MemoryInputStream RawInvokeProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult) override;
	};
}
