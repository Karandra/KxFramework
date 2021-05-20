#pragma once
#include "kxf/General/String.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/Serialization/BinarySerializer.h"
#include "kxf/Threading/Mutex.h"
#include "kxf/EventSystem/EventID.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/IO/MemoryStream.h"
#include "../SharedMemory.h"

namespace kxf
{
	class DefaultRPCEvent;
	class DefaultRPCProcedure;
	class DefaultRPCExchangerWindow;
}

namespace kxf
{
	class DefaultRPCExchanger
	{
		friend class DefaultRPCExchangerWindow;

		protected:
			GlobalMutex m_SessionMutex;
			SharedMemoryBuffer m_ControlBuffer;
			UniversallyUniqueID m_SessionID;

			SharedMemoryBuffer m_ResultBuffer;
			std::optional<MemoryInputStream> m_ResultStream;

			DefaultRPCExchangerWindow* m_ReceivingWindow = nullptr;
			IEvtHandler* m_EvtHandler = nullptr;

		protected:
			size_t GetControlBufferSize() const
			{
				return 64;
			}
			String GetControlBufferName() const
			{
				return String::Format(wxS("RPCSession:%1-ControlBuffer"), m_SessionID.ToString(UUIDFormat::CurlyBraces));
			}
			String GetResultBufferName() const
			{
				return String::Format(wxS("RPCSession:%1-ResultBuffer"), m_SessionID.ToString(UUIDFormat::CurlyBraces));
			}

			void OnInitialize(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler);
			void OnTerminate();

		protected:
			virtual void OnDataRecieved(IInputStream& stream) = 0;
			void OnDataRecievedCommon(IInputStream& stream, DefaultRPCEvent& event);

			IInputStream& SendData(void* windowHandle, const DefaultRPCProcedure& procedure, const wxStreamBuffer& buffer);
	};
}

namespace kxf
{
	class DefaultRPCProcedure final
	{
		friend struct BinarySerializer<DefaultRPCProcedure>;

		private:
			EventID m_ProcedureID;
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
			void* GetOriginHandle() const
			{
				return m_OriginHandle;
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
			return Serialization::WriteObject(stream, value.m_ProcedureID) +
				Serialization::WriteObject(stream, value.m_OriginHandle) +
				Serialization::WriteObject(stream, value.m_ParametersCount) +
				Serialization::WriteObject(stream, value.m_HasResult);
		}
		uint64_t Deserialize(IInputStream& stream, DefaultRPCProcedure& value) const
		{
			return Serialization::ReadObject(stream, value.m_ProcedureID) +
				Serialization::ReadObject(stream, value.m_OriginHandle) +
				Serialization::ReadObject(stream, value.m_ParametersCount) +
				Serialization::ReadObject(stream, value.m_HasResult);
		}
	};
}
