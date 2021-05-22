#include "KxfPCH.h"
#include "DefaultRPCServer.h"
#include "DefaultRPCEvent.h"
#include "Private/DefaultRPCExchangerWindow.h"
#include "kxf/Serialization/BinarySerializer.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/NullStream.h"

namespace kxf
{
	// DefaultRPCServer
	void DefaultRPCServer::Notify(const EventID& eventID)
	{
		DefaultRPCEvent event(*this);
		m_EvtHandler->ProcessEvent(event, eventID);
	}
	void DefaultRPCServer::NotifyClients(const EventID& eventID)
	{
		BroadcastProcedure(eventID);
	}
	void DefaultRPCServer::CleanupClients()
	{
		for (auto it = m_Clients.begin(); it != m_Clients.end();)
		{
			if (!::IsWindow(reinterpret_cast<HWND>(*it)))
			{
				it = m_Clients.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	bool DefaultRPCServer::DoStartServer(KernelObjectNamespace ns, bool notify)
	{
		if (m_SessionMutex.CreateAcquired(GetSessionMutexName(), ns) && m_ControlBuffer.Allocate(GetControlBufferSize(), MemoryProtection::RW, GetControlBufferName(), ns))
		{
			m_ReceivingWindow = new DefaultRPCExchangerWindow(*this, m_SessionID);

			// Write out everything that the client will need to connect to the server
			MemoryOutputStream stream = m_ControlBuffer.GetOutputStream();
			Serialization::WriteObject(stream, static_cast<uint32_t>(::GetCurrentProcessId()));
			Serialization::WriteObject(stream, reinterpret_cast<void*>(m_ReceivingWindow->GetHandle()));

			// Notify server started
			if (notify)
			{
				Notify(RPCEvent::EvtServerStarted);
				NotifyClients(RPCEvent::EvtServerStarted);
			}
			return true;
		}

		DoTerminateServer();
		return false;
	}
	void DefaultRPCServer::DoTerminateServer(bool notify)
	{
		if (m_SessionMutex && notify)
		{
			Notify(RPCEvent::EvtServerTerminated);
			NotifyClients(RPCEvent::EvtServerTerminated);
		}

		OnTerminate();
		m_UserEvtHandler = nullptr;
		m_ServiceEvtHandler.SetNextHandler(nullptr);
		m_Clients.clear();
	}

	// Private::DefaultRPCExchanger
	void DefaultRPCServer::OnDataRecieved(IInputStream& stream)
	{
		DefaultRPCEvent event(*this);
		DefaultRPCExchanger::OnDataRecievedCommon(stream, event);
	}

	DefaultRPCServer::DefaultRPCServer()
	{
		m_ServiceEvtHandler.Bind(RPCEvent::EvtClientConnected, [&](RPCEvent& event)
		{
			m_Clients.emplace(static_cast<DefaultRPCEvent&>(event).GetProcedure().GetOriginHandle());
		}, BindEventFlag::AlwaysSkip);
		m_ServiceEvtHandler.Bind(RPCEvent::EvtClientDisconnected, [&](RPCEvent& event)
		{
			m_Clients.erase(static_cast<DefaultRPCEvent&>(event).GetProcedure().GetOriginHandle());
		}, BindEventFlag::AlwaysSkip);
	}
	DefaultRPCServer::~DefaultRPCServer()
	{
		DoTerminateServer(true);
	}

	// IRPCServer
	bool DefaultRPCServer::IsServerRunning() const
	{
		return !m_SessionMutex.IsNull();
	}
	bool DefaultRPCServer::StartServer(const UniversallyUniqueID& sessionID, IEvtHandler& evtHandler, KernelObjectNamespace ns)
	{
		if (!m_SessionMutex)
		{
			m_UserEvtHandler = &evtHandler;
			m_ServiceEvtHandler.SetNextHandler(&evtHandler);

			OnInitialize(sessionID, m_ServiceEvtHandler, ns);
			return DoStartServer(ns, true);
		}
		return false;
	}
	void DefaultRPCServer::TerminateServer()
	{
		DoTerminateServer(true);
	}

	void DefaultRPCServer::RawBroadcastProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount)
	{
		CleanupClients();

		if (!m_Clients.empty() && m_SessionMutex && procedureID)
		{
			DefaultRPCProcedure procedure(procedureID, m_ReceivingWindow->GetHandle(), parametersCount);

			MemoryOutputStream stream;
			Serialization::WriteObject(stream, procedure);
			if (procedure.HasParameters())
			{
				stream.Write(parameters);
			}

			for (void* handle: m_Clients)
			{
				SendData(handle, procedure, stream.GetStreamBuffer());
			}
		}
	}
}
