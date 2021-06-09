#include "KxfPCH.h"
#include "DefaultRPCClient.h"
#include "DefaultRPCEvent.h"
#include "Private/DefaultRPCExchangerWindow.h"
#include "kxf/Log/Common.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/NullStream.h"
#include "kxf/IO/MemoryStream.h"
#include "kxf/Serialization/BinarySerializer.h"
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

	bool DefaultRPCClient::DoConnectToServer()
	{
		try
		{
			if (m_SessionMutex.Open(GetSessionMutexName(), m_KernelScope) && m_ControlBuffer.Open(GetControlBufferName(), 0, MemoryProtection::Read, m_KernelScope))
			{
				// Read control parameters
				const size_t initialControlBufferSize = GetControlBufferSize();
				MemoryInputStream stream = m_ControlBuffer.GetInputStreamUnchecked(initialControlBufferSize);

				uint64_t controlBufferSize = 0;
				Serialization::ReadObject(stream, controlBufferSize);
				if (controlBufferSize == initialControlBufferSize)
				{
					Serialization::ReadObject(stream, m_ServerPID);
					Serialization::ReadObject(stream, m_ServerHandle);

					if (::IsWindow(reinterpret_cast<HWND>(m_ServerHandle)) && m_ReceivingWindow.Create(m_SessionID))
					{
						Notify(RPCEvent::EvtClientConnected);
						NotifyServer(RPCEvent::EvtClientConnected);
						return true;
					}
				}
			}
		}
		catch (const BinarySerializerException& e)
		{
			Log::Error("Serialization exception: {}", e.what());
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
		if (m_SessionMutex)
		{
			DefaultRPCEvent event(*this);
			DefaultRPCExchanger::OnDataRecievedCommon(stream, event, m_ClientID);
		}
	}
	bool DefaultRPCClient::OnDataRecievedFilter(const DefaultRPCProcedure& procedure)
	{
		return procedure.m_OriginHandle == m_ServerHandle;
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
	bool DefaultRPCClient::ConnectToServer(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler, const UniversallyUniqueID& clientID, std::shared_ptr<IThreadPool> threadPool, FlagSet<RPCExchangeFlag> flags )
	{
		if (!m_SessionMutex)
		{
			m_ClientID = clientID;
			m_UserEvtHandler = &evtHandler;
			m_ServiceEvtHandler.SetNextHandler(&evtHandler);

			OnInitialize(sessionID, m_ServiceEvtHandler, std::move(threadPool), flags);
			return DoConnectToServer();
		}
		return false;
	}
	void DefaultRPCClient::DisconnectFromServer()
	{
		DoDisconnectFromServer(true);
	}

	MemoryInputStream DefaultRPCClient::RawInvokeProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult)
	{
		if (m_SessionMutex && procedureID)
		{
			DefaultRPCProcedure procedure(procedureID, m_ReceivingWindow.GetHandle(), parametersCount, hasResult);
			procedure.m_ClientID = m_ClientID;

			MemoryOutputStream stream;
			Serialization::WriteObject(stream, procedure);
			if (procedure.HasParameters())
			{
				stream.Write(parameters);
			}
			return SendData(m_ServerHandle, procedure, stream.GetStreamBuffer());
		}
		return {};
	}
}
