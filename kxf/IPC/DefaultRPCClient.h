#pragma once
#include "IRPCClient.h"
#include "Private/DefaultRPCExchanger.h"

namespace kxf
{
	class DefaultRPCClient: public RTTI::DynamicImplementation<DefaultRPCClient, IRPCClient>, public DefaultRPCExchanger
	{
		private:
			uint32_t m_ServerPID = 0;
			void* m_ServerHandle = nullptr;

		private:
			void Notify(const EventID& eventID);
			void NotifyServer(const EventID& eventID);

			bool DoConnectToServer(bool notify = false);
			void DoDisconnectFromServer(bool notify = false);

		protected:
			// Private::DefaultRPCExchanger
			void OnDataRecieved(IInputStream& stream) override;

		public:
			DefaultRPCClient() = default;
			~DefaultRPCClient()
			{
				DoDisconnectFromServer(true);
			}

		public:
			// IRPCClient
			bool IsConnectedToServer() const override;
			bool ConnectToServer(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler) override;
			void DisconnectFromServer() override;

			IInputStream& InvokeProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult) override;
	};
}
