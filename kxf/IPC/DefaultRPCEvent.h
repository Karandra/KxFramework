#pragma once
#include "RPCEvent.h"
#include "Private/DefaultRPCExchanger.h"

namespace kxf
{
	class DefaultRPCServer;
	class DefaultRPCClient;
}

namespace kxf
{
	class DefaultRPCEvent: public RPCEvent
	{
		friend class DefaultRPCExchanger;
		friend class DefaultRPCClient;
		friend class DefaultRPCServer;

		private:
			DefaultRPCProcedure m_Procedure;
			DefaultRPCServer* m_Server = nullptr;
			DefaultRPCClient* m_Client = nullptr;

			IInputStream* m_ParametersStream = nullptr;
			DataSize m_ParametersStreamOffset;
			MemoryOutputStream m_ResultStream;

		private:
			void RawSetParameters(IInputStream& stream);
			MemoryInputStream RawGetResult();

		public:
			DefaultRPCEvent() = default;
			DefaultRPCEvent(DefaultRPCServer& server)
				:m_Server(&server)
			{
			}
			DefaultRPCEvent(DefaultRPCClient& client)
				:m_Client(&client)
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

			// RPCEvent
			IRPCServer* GetServer() const override;
			IRPCClient* GetClient() const override;
			UniversallyUniqueID GetClientID() const override
			{
				return m_Procedure.GetClientID();
			}

			bool HasResult() const override
			{
				return m_Procedure.HasResult();
			}
			size_t GetParameterCount() const override
			{
				return m_Procedure.GetParametersCount();
			}
			IInputStream& RawGetParameters() override;
			void RawSetResult(IInputStream& stream) override;

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
