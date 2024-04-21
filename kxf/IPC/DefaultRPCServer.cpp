#include "KxfPCH.h"
#include "DefaultRPCServer.h"
#include "DefaultRPCEvent.h"
#include "Private/DefaultRPCExchangerWindow.h"
#include "kxf/Log/Common.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/NullStream.h"
#include "kxf/Serialization/BinarySerializer.h"

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

	void DefaultRPCServer::HandleClientEvent(DefaultRPCEvent& event, bool add)
	{
		const auto& procedure = event.GetProcedure();
		if (auto id = procedure.GetClientID(); !id.IsEmpty())
		{
			if (add)
			{
				if (m_UniqueClients.emplace(id, procedure.GetOriginHandle()).second)
				{
					event.Skip();
				}
			}
			else
			{
				if (m_UniqueClients.erase(id) != 0)
				{
					event.Skip();
				}
			}
		}
		else if (void* handle = procedure.GetOriginHandle())
		{
			if (add)
			{
				if (m_AnonymousClients.emplace(handle).second)
				{
					event.Skip();
				}
			}
			else
			{
				if (m_AnonymousClients.erase(handle) != 0)
				{
					event.Skip();
				}
			}
		}
	}
	void DefaultRPCServer::CleanupClients()
	{
		// Remove any invalid clients
		for (auto it = m_UniqueClients.begin(); it != m_UniqueClients.end();)
		{
			if (!::IsWindow(reinterpret_cast<HWND>(it->second)))
			{
				it = m_UniqueClients.erase(it);
			}
			else
			{
				++it;
			}
		}

		for (auto it = m_AnonymousClients.begin(); it != m_AnonymousClients.end();)
		{
			if (!::IsWindow(reinterpret_cast<HWND>(*it)))
			{
				it = m_AnonymousClients.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	bool DefaultRPCServer::DoStartServer()
	{
		try
		{
			size_t controlBufferSize = GetControlBufferSize();
			if (m_SessionMutex.CreateAcquired(GetSessionMutexName(), m_KernelScope) && m_ControlBuffer.Allocate(controlBufferSize, MemoryProtection::RW, GetControlBufferName(), m_KernelScope))
			{
				if (m_ReceivingWindow.Create(m_SessionID))
				{
					// Write out everything that the client will need to connect to the server
					MemoryOutputStream stream = m_ControlBuffer.GetOutputStream();
					Serialization::WriteObject(stream, static_cast<uint64_t>(controlBufferSize));
					Serialization::WriteObject(stream, static_cast<uint32_t>(::GetCurrentProcessId()));
					Serialization::WriteObject(stream, reinterpret_cast<void*>(m_ReceivingWindow.GetHandle()));

					// Notify server started
					Notify(RPCEvent::EvtServerStarted);
					NotifyClients(RPCEvent::EvtServerStarted);
					return true;
				}
			}
		}
		catch (const BinarySerializerException& e)
		{
			Log::Error("Serialization exception: {}", e.what());
		}

		DoTerminateServer(false);
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
		m_UniqueClients.clear();
		m_AnonymousClients.clear();
	}
	MemoryInputStream DefaultRPCServer::DoInvokeProcedure(const String& clientID, const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult)
	{
		CleanupClients();

		if (m_SessionMutex && procedureID)
		{
			auto PrepareData = [&](IOutputStream& stream)
			{
				DefaultRPCProcedure procedure(procedureID, m_ReceivingWindow.GetHandle(), parametersCount, hasResult);

				Serialization::WriteObject(stream, procedure);
				if (procedure.HasParameters())
				{
					stream.Write(parameters);
				}

				return procedure;
			};

			if (!clientID.IsEmpty())
			{
				auto it = m_UniqueClients.find(clientID);
				if (it != m_UniqueClients.end())
				{
					MemoryOutputStream stream;
					auto procedure = PrepareData(stream);

					return SendData(it->second, procedure, stream.GetStreamBuffer());
				}
			}
			else
			{
				MemoryOutputStream stream;
				auto procedure = PrepareData(stream);

				for (auto&& [id, handle]: m_UniqueClients)
				{
					SendData(handle, procedure, stream.GetStreamBuffer(), true);
				}
				for (void* handle: m_AnonymousClients)
				{
					SendData(handle, procedure, stream.GetStreamBuffer(), true);
				}
			}
		}
		return {};
	}

	// Private::DefaultRPCExchanger
	void DefaultRPCServer::OnDataRecieved(IInputStream& stream)
	{
		if (m_SessionMutex)
		{
			DefaultRPCEvent event(*this);
			DefaultRPCExchanger::OnDataRecievedCommon(stream, event);
		}
	}
	bool DefaultRPCServer::OnDataRecievedFilter(const DefaultRPCProcedure& procedure)
	{
		if (!procedure.m_ClientID.IsEmpty())
		{
			auto it = m_UniqueClients.find(procedure.m_ClientID);
			if (it != m_UniqueClients.end())
			{
				return it->second == procedure.m_OriginHandle;
			}
			else
			{
				return procedure.m_ProcedureID == RPCEvent::EvtClientConnected;
			}
		}
		else
		{
			return m_AnonymousClients.count(procedure.m_OriginHandle) != 0 || procedure.m_ProcedureID == RPCEvent::EvtClientConnected;
		}
		return false;
	}

	DefaultRPCServer::DefaultRPCServer()
	{
		m_ServiceEvtHandler.Bind(RPCEvent::EvtClientConnected, [&](RPCEvent& event)
		{
			HandleClientEvent(static_cast<DefaultRPCEvent&>(event), true);
		});
		m_ServiceEvtHandler.Bind(RPCEvent::EvtClientDisconnected, [&](RPCEvent& event)
		{
			HandleClientEvent(static_cast<DefaultRPCEvent&>(event), false);
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
	bool DefaultRPCServer::StartServer(const String& sessionID, IEvtHandler& evtHandler, std::shared_ptr<IThreadPool> threadPool, FlagSet<RPCExchangeFlag> flags)
	{
		if (!m_SessionMutex)
		{
			m_UserEvtHandler = &evtHandler;
			m_ServiceEvtHandler.SetNextHandler(&evtHandler);

			OnInitialize(sessionID, m_ServiceEvtHandler, std::move(threadPool), flags);
			return DoStartServer();
		}
		return false;
	}
	void DefaultRPCServer::TerminateServer()
	{
		DoTerminateServer(true);
	}

	MemoryInputStream DefaultRPCServer::RawInvokeProcedure(const String& clientID, const EventID& procedureID, IInputStream& parameters, size_t parametersCount, bool hasResult)
	{
		if (!clientID.IsEmpty())
		{
			return DoInvokeProcedure(clientID, procedureID, parameters, parametersCount, hasResult);
		}
		return {};
	}
	void DefaultRPCServer::RawBroadcastProcedure(const EventID& procedureID, IInputStream& parameters, size_t parametersCount)
	{
		DoInvokeProcedure({}, procedureID, parameters, parametersCount, false);
	}
}
