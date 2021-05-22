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
		InvokeProcedure(eventID);
	}

	bool DefaultRPCClient::DoConnectToServer(KernelObjectNamespace ns, bool notify)
	{
		if (m_SessionMutex.Open(GetSessionMutexName(), ns) && m_ControlBuffer.Open(GetControlBufferName(), GetControlBufferSize(), MemoryProtection::Read, ns))
		{
			// Read control parameters
			auto stream = m_ControlBuffer.GetInputStream();
			Serialization::ReadObject(*stream, m_ServerPID);
			Serialization::ReadObject(*stream, m_ServerHandle);

			if (::IsWindow(reinterpret_cast<HWND>(m_ServerHandle)))
			{
				m_ReceivingWindow = new DefaultRPCExchangerWindow(*this, m_SessionID);

				if (notify)
				{
					Notify(RPCEvent::EvtClientConnected);
					NotifyServer(RPCEvent::EvtClientConnected);
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
			Notify(RPCEvent::EvtClientDisconnected);
			NotifyServer(RPCEvent::EvtClientDisconnected);
		}

		m_ServerPID = 0;
		m_ServerHandle = nullptr;
		m_UserEvtHandler = nullptr;
		m_ServiceEvtHandler.SetNextHandler(nullptr);
		OnTerminate();
	}

	// Private::DefaultRPCExchanger
	void DefaultRPCClient::OnDataRecieved(IInputStream& stream)
	{
		DefaultRPCEvent event(*this);
		DefaultRPCExchanger::OnDataRecievedCommon(stream, event);
	}

	DefaultRPCClient::DefaultRPCClient()
	{
		// TODO: Watch server status using the provided PID and initiate disconnect event
		// the server terminates without proper notifications (i.e crashes).
	}
	DefaultRPCClient::~DefaultRPCClient()
	{
		DoDisconnectFromServer(true);
	}

	// IRPCClient
	bool DefaultRPCClient::IsConnectedToServer() const
	{
		return !m_SessionMutex.IsNull();
	}
	bool DefaultRPCClient::ConnectToServer(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler, KernelObjectNamespace ns)
	{
		if (!m_SessionMutex)
		{
			m_UserEvtHandler = &evtHandler;
			m_ServiceEvtHandler.SetNextHandler(&evtHandler);

			OnInitialize(sessionID, m_ServiceEvtHandler, ns);
			return DoConnectToServer(ns, true);
		}
		return false;
	}
	void DefaultRPCClient::DisconnectFromServer()
	{
		DoDisconnectFromServer(true);
	}

	IInputStream& DefaultRPCClient::RawInvokeProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult)
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
