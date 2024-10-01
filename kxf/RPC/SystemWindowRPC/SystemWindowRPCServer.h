#pragma once
#include "../IRPCServer.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "Private/SystemWindowRPCExchanger.h"

namespace kxf
{
	class SystemWindowRPCEvent;
}

namespace kxf
{
	class SystemWindowRPCServer: public RTTI::DynamicImplementation<SystemWindowRPCServer, IRPCServer>, public SystemWindowRPCExchanger
	{
		private:
			EvtHandler m_ServiceEvtHandler;
			IEvtHandler* m_UserEvtHandler = nullptr;

			std::unordered_map<String, SystemWindow> m_UniqueClients;
			std::unordered_set<SystemWindow> m_AnonymousClients;

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
			void HandleClientEvent(SystemWindowRPCEvent& event, bool add);
			void CleanupClients();

		protected:
			// SystemWindowRPCExchanger
			void OnDataRecieved(IInputStream& stream) override;
			bool OnDataRecievedFilter(const SystemWindowRPCProcedure& procedure) override;

		public:
			SystemWindowRPCServer();
			~SystemWindowRPCServer();

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

			// SystemWindowRPCServer
			size_t EnumUniqueClients(std::move_only_function<CallbackCommand(const String&, SystemWindow)> func) const;
			size_t EnumAnonymousClients(std::move_only_function<CallbackCommand(SystemWindow)> func) const;
	};
}
