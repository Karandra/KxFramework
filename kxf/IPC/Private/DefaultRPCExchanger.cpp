#include "KxfPCH.h"
#include "DefaultRPCExchanger.h"
#include "DefaultRPCExchangerWindow.h"
#include "../DefaultRPCEvent.h"
#include "../SharedMemory.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/NullStream.h"
#include "kxf/System/Win32Error.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/Application/ICoreApplication.h"
#include <Windows.h>

namespace
{
	constexpr kxf::XChar g_SharedPrefix[] = wxS("kxf::DefaultRPCExchanger");
}

namespace kxf
{
	size_t DefaultRPCExchanger::GetControlBufferSize() const
	{
		return 64;
	}
	String DefaultRPCExchanger::GetControlBufferName() const
	{
		return String::Format(wxS("%1:%2-ControlBuffer"), g_SharedPrefix, m_SessionID.ToString(UUIDFormat::CurlyBraces));
	}
	String DefaultRPCExchanger::GetResultBufferName() const
	{
		return String::Format(wxS("%1:%2-ResultBuffer"), g_SharedPrefix, m_SessionID.ToString(UUIDFormat::CurlyBraces));
	}

	void DefaultRPCExchanger::OnInitialize(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler)
	{
		m_SessionID = sessionID;
		m_EvtHandler = &evtHandler;
	}
	void DefaultRPCExchanger::OnTerminate()
	{
		if (m_ReceivingWindow)
		{
			m_ReceivingWindow->Destroy();
			m_ReceivingWindow = nullptr;
		}
		m_SessionMutex.Destroy();
		m_ControlBuffer.Free();
	}

	void DefaultRPCExchanger::OnDataRecievedCommon(IInputStream& stream, DefaultRPCEvent& event)
	{
		auto& procedure = event.m_Procedure;
		Serialization::ReadObject(stream, procedure);

		if (procedure)
		{
			// Set stream with serialized parameters for an event handler to read from
			if (procedure.HasParameters())
			{
				event.RawSetProcedureParameters(stream);
			}

			// Call event handler if any
			if (m_EvtHandler->ProcessEvent(event, procedure.GetProcedureID()) && procedure.HasResult())
			{
				// If we had processed the event get serialized result and write it into shared result buffer
				if (IInputStream& resultStream = event.RawGetProcedureResult())
				{
					m_ResultBuffer.AllocateGlobal(resultStream.GetSize().ToBytes(), MemoryProtection::RW, GetResultBufferName());
					m_ResultBuffer.GetOutputStream()->Write(resultStream);
				}
			}
		}
	}
	IInputStream& DefaultRPCExchanger::SendData(void* windowHandle, const DefaultRPCProcedure& procedure, const wxStreamBuffer& buffer)
	{
		COPYDATASTRUCT data = {};
		data.lpData = buffer.GetBufferStart();
		data.cbData = buffer.GetBufferSize();

		m_ResultStream.reset();
		::SendMessageW(reinterpret_cast<HWND>(windowHandle), WM_COPYDATA, 0, reinterpret_cast<LPARAM>(&data));

		if (procedure.HasResult() && Win32Error::GetLastError().IsSuccess())
		{
			m_ResultStream.emplace(m_ResultBuffer.GetBuffer(), m_ResultBuffer.GetSize());
			return *m_ResultStream;
		}
		return NullInputStream::Get();
	}
}
