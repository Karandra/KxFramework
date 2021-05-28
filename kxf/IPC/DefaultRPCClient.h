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
			UniversallyUniqueID m_ClientID;

			uint32_t m_ServerPID = 0;
			void* m_ServerHandle = nullptr;

		private:
			void Notify(const EventID& eventID);
			void NotifyServer(const EventID& eventID);

			bool DoConnectToServer();
			void DoDisconnectFromServer(bool notify);

		protected:
			// Private::DefaultRPCExchanger
			void OnDataRecieved(IInputStream& stream) override;
			bool OnDataRecievedFilter(const DefaultRPCProcedure& procedure) override;

		public:
			DefaultRPCClient();
			~DefaultRPCClient();

		public:
			// IRPCClient
			bool IsConnectedToServer() const override;
			bool ConnectToServer(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler, const UniversallyUniqueID& clientID = {}, std::shared_ptr<IThreadPool> threadPool = {}, FlagSet<RPCExchangeFlag> flags = {}) override;
			void DisconnectFromServer() override;
			UniversallyUniqueID GetSessionID() const override
			{
				return m_SessionID;
			}
			UniversallyUniqueID GetClientID() const override
			{
				return m_ClientID;
			}

			MemoryInputStream RawInvokeProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult) override;
	};
}
