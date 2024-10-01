#pragma once
#include "../RPCEvent.h"
#include "Private/SystemWindowRPCExchanger.h"

namespace kxf
{
	class SystemWindowRPCServer;
	class SystemWindowRPCClient;
}

namespace kxf
{
	class SystemWindowRPCEvent: public RPCEvent
	{
		friend class SystemWindowRPCExchanger;
		friend class SystemWindowRPCClient;
		friend class SystemWindowRPCServer;

		private:
			SystemWindowRPCProcedure m_Procedure;
			SystemWindowRPCServer* m_Server = nullptr;
			SystemWindowRPCClient* m_Client = nullptr;

			IInputStream* m_ParametersStream = nullptr;
			DataSize m_ParametersStreamOffset;
			MemoryOutputStream m_ResultStream;

		private:
			void RawSetParameters(IInputStream& stream);
			MemoryInputStream RawGetResult();

		public:
			SystemWindowRPCEvent() = default;
			SystemWindowRPCEvent(SystemWindowRPCServer& server)
				:m_Server(&server)
			{
			}
			SystemWindowRPCEvent(SystemWindowRPCClient& client)
				:m_Client(&client)
			{
			}

			SystemWindowRPCEvent(const SystemWindowRPCEvent&) = delete;
			SystemWindowRPCEvent(SystemWindowRPCEvent&& other) noexcept
			{
				*this = std::move(other);
			}
			~SystemWindowRPCEvent() = default;

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<SystemWindowRPCEvent>(std::move(*this));
			}

			// RPCEvent
			IRPCServer* GetServer() const override;
			IRPCClient* GetClient() const override;
			String GetClientID() const override
			{
				return m_Procedure.GetClientID();
			}

			bool HasResult() const override
			{
				return m_Procedure.HasResult();
			}
			size_t GetParameterCount() const override
			{
				return m_Procedure.GetParameterCount();
			}
			IInputStream& RawGetParameters() override;
			void RawSetResult(IInputStream& stream) override;

			// SystemWindowRPCEvent
			const SystemWindowRPCProcedure& GetProcedure() const&
			{
				return m_Procedure;
			}
			SystemWindowRPCProcedure GetProcedure() &&
			{
				return std::move(m_Procedure);
			}

		public:
			SystemWindowRPCEvent& operator=(const SystemWindowRPCEvent&) = delete;
			SystemWindowRPCEvent& operator=(SystemWindowRPCEvent&& other) noexcept
			{
				static_cast<BasicEvent&>(*this) = std::move(other);
				m_Procedure = std::move(other.m_Procedure);

				m_Server = other.m_Server;
				other.m_Server = nullptr;

				m_Client = other.m_Client;
				other.m_Client = nullptr;

				return *this;
			}
	};
}
