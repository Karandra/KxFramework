#pragma once
#include "IRPCEvent.h"
#include "Private/DefaultRPCExchanger.h"

namespace kxf
{
	class DefaultRPCServer;
	class DefaultRPCClient;
}

namespace kxf
{
	class DefaultRPCEvent: public IRPCEvent
	{
		friend class DefaultRPCExchanger;

		private:
			DefaultRPCProcedure m_Procedure;
			DefaultRPCServer* m_Server = nullptr;
			DefaultRPCClient* m_Client = nullptr;

			IInputStream* m_ParametersStream = nullptr;
			StreamOffset m_ParametersStreamOffset;
			std::optional<MemoryOutputStream> m_ResultStream;
			std::optional<MemoryInputStream> m_ResultStreamRead;

		public:
			DefaultRPCEvent() = default;
			DefaultRPCEvent(DefaultRPCServer& server, DefaultRPCProcedure procedure = {})
				:m_Procedure(std::move(procedure)), m_Server(&server)
			{
			}
			DefaultRPCEvent(DefaultRPCClient& client, DefaultRPCProcedure procedure = {})
				:m_Procedure(std::move(procedure)), m_Client(&client)
			{
			}

			DefaultRPCEvent(const DefaultRPCEvent&) = delete;
			DefaultRPCEvent(DefaultRPCEvent&& other) noexcept
			{
				*this = std::move(other);
			}
			~DefaultRPCEvent() = default;

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<DefaultRPCEvent>(std::move(*this));
			}

			// IRPCEvent
			IRPCServer* GetServer() const override;
			IRPCClient* GetClient() const override;

			IInputStream& RawGetProcedureResult() override;
			void RawSetProcedureResult(IInputStream& stream) override;

			IInputStream& RawGetProcedureParameters() override;
			void RawSetProcedureParameters(IInputStream& stream) override;

			// DefaultRPCEvent
			const DefaultRPCProcedure& GetProcedure() const&
			{
				return m_Procedure;
			}
			DefaultRPCProcedure GetProcedure() &&
			{
				return std::move(m_Procedure);
			}

		public:
			DefaultRPCEvent& operator=(const DefaultRPCEvent&) = delete;
			DefaultRPCEvent& operator=(DefaultRPCEvent&& other) noexcept
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
