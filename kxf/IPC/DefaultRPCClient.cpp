#include "KxfPCH.h"
#include "DefaultRPCClient.h"
#include "DefaultRPCEvent.h"
#include "Private/DefaultRPCExchangerWindow.h"
#include "kxf/Serialization/BinarySerializer.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/NullStream.h"
#include "kxf/IO/MemoryStream.h"
#include <Windows.h>

namespace kxf
{
	// DefaultRPCClient
	void DefaultRPCClient::Notify(const EventID& eventID)
	{
		DefaultRPCEvent event(*this);
		m_EvtHandler->ProcessEvent(event, eventID);
	}
	void DefaultRPCClient::NotifyServer(const EventID& eventID)
	{
		DefaultRPCClient::InvokeProcedure(eventID, NullInputStream::Get(), 0, false);
	}

	bool DefaultRPCClient::DoConnectToServer(bool notify)
	{
		String name = GetControlBufferName();
		if (m_SessionMutex.Open(name) && m_ControlBuffer.OpenGlobal(name, GetControlBufferSize(), MemoryProtection::Read))
		{
			// Read control parameters
			auto stream = m_ControlBuffer.GetInputStream();
			Serialization::ReadObject(*stream, m_ServerPID);
			Serialization::ReadObject(*stream, m_ServerHandle);

			if (::IsWindow(reinterpret_cast<HWND>(m_ServerHandle)))
			{
				if (notify)
				{
					Notify(IRPCEvent::EvtClientConnected);
					NotifyServer(IRPCEvent::EvtClientConnected);
				}
				return true;
			}
		}

		DoDisconnectFromServer(false);
		return false;
	}
	void DefaultRPCClient::DoDisconnectFromServer(bool notify)
	{
		if (m_SessionMutex && notify)
		{
			Notify(IRPCEvent::EvtClientDisconnected);
			NotifyServer(IRPCEvent::EvtClientDisconnected);
		}
		OnTerminate();
	}

	// Private::DefaultRPCExchanger
	void DefaultRPCClient::OnDataRecieved(IInputStream& stream)
	{
		DefaultRPCEvent event(*this);
		DefaultRPCExchanger::OnDataRecievedCommon(stream, event);
	}

	// IRPCClient
	bool DefaultRPCClient::IsConnectedToServer() const
	{
		return !m_SessionMutex.IsNull();
	}
	bool DefaultRPCClient::ConnectToServer(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler)
	{
		if (!m_SessionMutex)
		{
			OnInitialize(sessionID, evtHandler);
			return DoConnectToServer();
		}
		return false;
	}
	void DefaultRPCClient::DisconnectFromServer()
	{
		DoDisconnectFromServer(true);
	}

	IInputStream& DefaultRPCClient::InvokeProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult)
	{
		if (m_SessionMutex && procedureID)
		{
			DefaultRPCProcedure procedure(procedureID, m_ReceivingWindow->GetHandle(), parametersCount, hasResult);

			MemoryOutputStream stream;
			Serialization::WriteObject(stream, procedure);
			if (procedure.HasParameters())
			{
				stream.Write(parameters);
			}
			return SendData(m_ServerHandle, procedure, stream.GetStreamBuffer());
		}
		return NullInputStream::Get();
	}
}
