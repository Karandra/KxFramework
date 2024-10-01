#pragma once
#include "kxf/Core/String.h"
#include "kxf/Core/UniversallyUniqueID.h"
#include "kxf/Serialization/BinarySerializer.h"
#include "kxf/Threading/Mutex.h"
#include "kxf/EventSystem/EventID.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/IO/MemoryStream.h"
#include "kxf/RPC/SharedMemory.h"
#include "SystemWindowRPCExchangerWindow.h"

namespace kxf
{
	class IThreadPool;
	class SystemWindowRPCEvent;
	class SystemWindowRPCProcedure;
}

namespace kxf
{
	class SystemWindowRPCExchanger
	{
		friend class SystemWindowRPCExchangerWindow;

		protected:
			Mutex m_SessionMutex;
			SharedMemoryBuffer m_ControlBuffer;
			String m_SessionID;
			FlagSet<RPCExchangeFlag> m_ExchangeFlags;
			KernelObjectNamespace m_KernelScope = KernelObjectNamespace::None;

			SharedMemoryBuffer m_ResultBuffer;
			SystemWindowRPCExchangerWindow m_ReceivingWindow;
			IEvtHandler* m_EvtHandler = nullptr;

		protected:
			SystemWindowRPCExchanger()
				:m_ReceivingWindow(*this)
			{
			}

		public:
			size_t GetControlBufferSize() const;
			String GetControlBufferName() const;
			String GetResultBufferName() const;
			String GetSessionMutexName() const;

			void OnInitialize(const String& sessionID, IEvtHandler& evtHandler, std::shared_ptr<IThreadPool> threadPool, FlagSet<RPCExchangeFlag> flags);
			void OnTerminate();

		public:
			virtual void OnDataRecieved(IInputStream& stream) = 0;
			virtual bool OnDataRecievedFilter(const SystemWindowRPCProcedure& procedure) = 0;
			void OnDataRecievedCommon(IInputStream& stream, SystemWindowRPCEvent& event, const String& clientID = {});

			MemoryInputStream SendData(void* windowHandle, const SystemWindowRPCProcedure& procedure, const MemoryStreamBuffer& buffer, bool discardResult = false);
	};
}

namespace kxf
{
	class SystemWindowRPCProcedure final
	{
		friend struct BinarySerializer<SystemWindowRPCProcedure>;
		friend class SystemWindowRPCExchanger;
		friend class SystemWindowRPCClient;
		friend class SystemWindowRPCServer;

		public:
			static constexpr uint32_t GetFormatVersion() noexcept
			{
				return 1;
			}

		private:
			uint32_t m_Version = GetFormatVersion();
			EventID m_ProcedureID;
			String m_ClientID;
			void* m_OriginHandle = nullptr;
			uint32_t m_ParametersCount = 0;
			bool m_HasResult = false;

		public:
			SystemWindowRPCProcedure() noexcept = default;
			SystemWindowRPCProcedure(EventID procedureID, void* originHandle, size_t parametersCount, bool hasResult = false) noexcept
				:m_ProcedureID(std::move(procedureID)), m_OriginHandle(originHandle), m_ParametersCount(static_cast<uint32_t>(parametersCount)), m_HasResult(hasResult)
			{
			}

		public:
			bool IsNull() const noexcept
			{
				return m_ProcedureID.IsNull();
			}
			bool HasResult() const noexcept
			{
				return m_HasResult;
			}
			bool HasParameters() const noexcept
			{
				return m_ParametersCount != 0;
			}
			uint32_t GetParametersCount() const noexcept
			{
				return m_ParametersCount;
			}

			const EventID& GetProcedureID() const& noexcept
			{
				return m_ProcedureID;
			}
			EventID GetProcedureID() && noexcept
			{
				return std::move(m_ProcedureID);
			}

			uint32_t GetVersion() const
			{
				return m_Version;
			}
			void* GetOriginHandle() const
			{
				return m_OriginHandle;
			}

			String GetClientID() const
			{
				return m_ClientID;
			}
			void SetClientID(String clientID)
			{
				m_ClientID = std::move(clientID);
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<SystemWindowRPCProcedure> final
	{
		uint64_t Serialize(IOutputStream& stream, const SystemWindowRPCProcedure& value) const
		{
			return Serialization::WriteObject(stream, value.m_Version) +
				Serialization::WriteObject(stream, value.m_ProcedureID) +
				Serialization::WriteObject(stream, value.m_ClientID) +
				Serialization::WriteObject(stream, value.m_OriginHandle) +
				Serialization::WriteObject(stream, value.m_ParametersCount) +
				Serialization::WriteObject(stream, value.m_HasResult);
		}
		uint64_t Deserialize(IInputStream& stream, SystemWindowRPCProcedure& value) const
		{
			auto read = Serialization::ReadObject(stream, value.m_Version);
			if (value.m_Version != SystemWindowRPCProcedure::GetFormatVersion())
			{
				throw BinarySerializerException(Format("Unsupported version encountered during 'SystemWindowRPCProcedure' deserialization: {} found, {} expected",
												value.m_Version,
												SystemWindowRPCProcedure::GetFormatVersion())
				);
			}

			read += Serialization::ReadObject(stream, value.m_ProcedureID);
			read += Serialization::ReadObject(stream, value.m_ClientID);
			read += Serialization::ReadObject(stream, value.m_OriginHandle);
			read += Serialization::ReadObject(stream, value.m_ParametersCount);
			read += Serialization::ReadObject(stream, value.m_HasResult);
			return read;
		}
	};
}
