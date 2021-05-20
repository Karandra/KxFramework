#pragma once
#include "IRPCServer.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "Private/DefaultRPCExchanger.h"

namespace kxf::Private
{
	class DefaultRPCExchangerWindow;
}

namespace kxf
{
	class DefaultRPCServer: public RTTI::DynamicImplementation<DefaultRPCServer, IRPCServer>, public DefaultRPCExchanger
	{
		private:
			EvtHandler m_ServiceEvtHandler;
			IEvtHandler* m_UserEvtHandler = nullptr;

			std::set<void*> m_Clients;

		private:
			void Notify(const EventID& eventID);
			void NotifyClients(const EventID& eventID);

			bool DoStartServer(bool notify = false);
			void DoTerminateServer(bool notify = false);

		protected:
			// Private::DefaultRPCExchanger
			void OnDataRecieved(IInputStream& stream) override;

		public:
			DefaultRPCServer();
			~DefaultRPCServer()
			{
				DoTerminateServer(true);
			}

		public:
			// IRPCServer
			bool IsServerRunning() const override;
			bool StartServer(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler) override;
			void TerminateServer() override;

			void BreadcastProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount) override;
	};
}
