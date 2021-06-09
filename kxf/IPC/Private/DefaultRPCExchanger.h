#pragma once
#include "kxf/General/String.h"
#include "kxf/General/Format.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/Serialization/BinarySerializer.h"
#include "kxf/Threading/Mutex.h"
#include "kxf/EventSystem/EventID.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/IO/MemoryStream.h"
#include "../SharedMemory.h"
#include "DefaultRPCExchangerWindow.h"

namespace kxf
{
	class IThreadPool;
	class DefaultRPCEvent;
	class DefaultRPCProcedure;
}

namespace kxf
{
	class DefaultRPCExchanger
	{
		friend class DefaultRPCExchangerWindow;

		protected:
			Mutex m_SessionMutex;
			SharedMemoryBuffer m_ControlBuffer;
			UniversallyUniqueID m_SessionID;
			FlagSet<RPCExchangeFlag> m_ExchangeFlags;
			KernelObjectNamespace m_KernelScope = KernelObjectNamespace::None;

			SharedMemoryBuffer m_ResultBuffer;
			DefaultRPCExchangerWindow m_ReceivingWindow;
			IEvtHandler* m_EvtHandler = nullptr;

		protected:
			DefaultRPCExchanger()
				:m_ReceivingWindow(*this)
			{
			}

		public:
			size_t GetControlBufferSize() const;
			String GetControlBufferName() const;
			String GetResultBufferName() const;
			String GetSessionMutexName() const;

			void OnInitialize(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler, std::shared_ptr<IThreadPool> threadPool, FlagSet<RPCExchangeFlag> flags);
			void OnTerminate();

		public:
			virtual void OnDataRecieved(IInputStream& stream) = 0;
			virtual bool OnDataRecievedFilter(const DefaultRPCProcedure& procedure) = 0;
			void OnDataRecievedCommon(IInputStream& stream, DefaultRPCEvent& event, const UniversallyUniqueID& clientID = {});

			MemoryInputStream SendData(void* windowHandle, const DefaultRPCProcedure& procedure, const MemoryStreamBuffer& buffer, bool discardResult = false);
	};
}

namespace kxf
{
	class DefaultRPCProcedure final
	{
		friend struct BinarySerializer<DefaultRPCProcedure>;
		friend class DefaultRPCExchanger;
		friend class DefaultRPCClient;
		friend class DefaultRPCServer;

		public:
			static constexpr uint32_t GetFormatVersion() noexcept
			{
				return 1;
			}

		private:
			uint32_t m_Version = GetFormatVersion();
			EventID m_ProcedureID;
			UniversallyUniqueID m_ClientID;
			void* m_OriginHandle = nullptr;
			uint32_t m_ParametersCount = 0;
			bool m_HasResult = false;

		public:
			DefaultRPCProcedure() noexcept = default;
			DefaultRPCProcedure(EventID procedureID, void* originHandle, size_t parametersCount, bool hasResult = false) noexcept
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

			UniversallyUniqueID GetClientID() const
			{
				return m_ClientID;
			}
			void SetClientID(const UniversallyUniqueID& clientID)
			{
				m_ClientID = clientID;
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
	struct BinarySerializer<DefaultRPCProcedure> final
	{
		uint64_t Serialize(IOutputStream& stream, const DefaultRPCProcedure& value) const
		{
			return Serialization::WriteObject(stream, value.m_Version) +
				Serialization::WriteObject(stream, value.m_ProcedureID) +
				Serialization::WriteObject(stream, value.m_ClientID) +
				Serialization::WriteObject(stream, value.m_OriginHandle) +
				Serialization::WriteObject(stream, value.m_ParametersCount) +
				Serialization::WriteObject(stream, value.m_HasResult);
		}
		uint64_t Deserialize(IInputStream& stream, DefaultRPCProcedure& value) const
		{
			auto read = Serialization::ReadObject(stream, value.m_Version);
			if (value.m_Version != DefaultRPCProcedure::GetFormatVersion())
			{
				throw BinarySerializerException(Format("Unsupported version encountered during 'DefaultRPCProcedure' deserialization: {} found, {} expected",
												value.m_Version,
												DefaultRPCProcedure::GetFormatVersion())
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
