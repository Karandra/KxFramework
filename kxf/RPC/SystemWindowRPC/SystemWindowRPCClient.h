#pragma once
#include "../IRPCClient.h"
#include "kxf/System/SystemWindow.h"
#include "kxf/System/SystemProcess.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "Private/SystemWindowRPCExchanger.h"

namespace kxf
{
	class SystemWindowRPCClient: public RTTI::DynamicImplementation<SystemWindowRPCClient, IRPCClient>, public SystemWindowRPCExchanger
	{
		private:
			EvtHandler m_ServiceEvtHandler;
			IEvtHandler* m_UserEvtHandler = nullptr;
			String m_ClientID;

			uint32_t m_ServerPID = 0;
			void* m_ServerHandle = nullptr;

		private:
			void Notify(const EventID& eventID);
			void NotifyServer(const EventID& eventID);

			bool DoConnectToServer();
			void DoDisconnectFromServer(bool notify);

		protected:
			// SystemWindowRPCExchanger
			void OnDataRecieved(IInputStream& stream) override;
			bool OnDataRecievedFilter(const SystemWindowRPCProcedure& procedure) override;

		public:
			SystemWindowRPCClient();
			~SystemWindowRPCClient();

		public:
			// IRPCClient
			bool IsConnectedToServer() const override;
			bool ConnectToServer(const String& sessionID, IEvtHandler& evtHandler, const String& clientID = {}, std::shared_ptr<IThreadPool> threadPool = {}, FlagSet<RPCExchangeFlag> flags = {}) override;
			void DisconnectFromServer() override;
			String GetSessionID() const override
			{
				return m_SessionID;
			}
			String GetClientID() const override
			{
				return m_ClientID;
			}

			MemoryInputStream RawInvokeProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult) override;

			// SystemWindowRPCClient
			SystemProcess GetServerProcess() const noexcept
			{
				return m_ServerPID;
			}
			SystemWindow GetServerRecievingWindow() const noexcept
			{
				return m_ServerHandle;
			}
	};
}
