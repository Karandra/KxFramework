#pragma once
#include "IRPCServer.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "Private/DefaultRPCExchanger.h"

namespace kxf
{
	class DefaultRPCEvent;
}

namespace kxf
{
	class DefaultRPCServer: public RTTI::DynamicImplementation<DefaultRPCServer, IRPCServer>, public DefaultRPCExchanger
	{
		private:
			EvtHandler m_ServiceEvtHandler;
			IEvtHandler* m_UserEvtHandler = nullptr;

			std::unordered_map<String, void*> m_UniqueClients;
			std::unordered_set<void*> m_AnonymousClients;

		private:
			void Notify(const EventID& eventID);
			void NotifyClients(const EventID& eventID);

			bool DoStartServer();
			void DoTerminateServer(bool notify);
			MemoryInputStream DoInvokeProcedure(const String& clientID, const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult);

			bool HasAnyClients() const noexcept
			{
				return m_UniqueClients.size() != 0 || m_AnonymousClients.size() != 0;
			}
			void HandleClientEvent(DefaultRPCEvent& event, bool add);
			void CleanupClients();

		protected:
			// Private::DefaultRPCExchanger
			void OnDataRecieved(IInputStream& stream) override;
			bool OnDataRecievedFilter(const DefaultRPCProcedure& procedure) override;

		public:
			DefaultRPCServer();
			~DefaultRPCServer();

		public:
			// IRPCServer
			bool IsServerRunning() const override;
			bool StartServer(const String& sessionID, IEvtHandler& evtHandler, std::shared_ptr<IThreadPool> threadPool = {}, FlagSet<RPCExchangeFlag> flags = {}) override;
			void TerminateServer() override;
			String GetSessionID() const override
			{
				return m_SessionID;
			}

			MemoryInputStream RawInvokeProcedure(const String& clientID, const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult) override;
			void RawBroadcastProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount) override;
	};
}
