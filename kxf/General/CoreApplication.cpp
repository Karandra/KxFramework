#include "stdafx.h"
#include "CoreApplication.h"
#include "kxf/EventSystem/IEventExecutor.h"
#include "kxf/EventSystem/IdleEvent.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/Utility/Container.h"
#include "kxf/Utility/CallAtScopeExit.h"

namespace kxf
{
	// CoreApplication
	void CoreApplication::OnExit()
	{
		// Finalize scheduled objects if there are anything left
		FinalizeScheduledForDestruction();
	}
	int CoreApplication::OnRun()
	{
		// TODO: Create main loop
	}

	void CoreApplication::Exit(int exitCode)
	{
		if (m_MainLoop)
		{
			ExitMainLoop(exitCode);
		}
		else
		{
			m_ExitCode = exitCode;
			std::terminate();
		}
	}

	// Application::IBasicInfo
	String CoreApplication::GetName() const
	{
		if (!m_Name.IsEmpty())
		{
			return m_Name;
		}
		return DynamicLibrary::GetCurrentModule().GetFilePath().GetName();
	}
	String CoreApplication::GetDisplayName() const
	{
		if (!m_DisplayName.IsEmpty())
		{
			return m_DisplayName;
		}
		else if (!m_Name.IsEmpty())
		{
			return m_Name;
		}
		return GetName().MakeCapitalized();
	}
	String CoreApplication::GetVendorName() const
	{
		return m_NativeApp.GetVendorName();
	}

	// Application::IMainEventLoop
	int CoreApplication::MainLoop()
	{

	}
	void CoreApplication::ExitMainLoop(int exitCode)
	{
		// We should exit from the main event loop, not just any currently active (e.g. modal dialog) event loop
		m_ExitCode = exitCode;
		if (m_MainLoop && m_MainLoop->IsRunning())
		{
			m_MainLoop->Exit(exitCode);
		}
	}

	// Application::IActiveEventLoop
	void CoreApplication::WakeUp()
	{
		if (IEventLoop* eventLoop = IEventLoop::GetActive())
		{
			eventLoop->WakeUp();
		}
	}
	bool CoreApplication::Pending()
	{
		if (IEventLoop* eventLoop = IEventLoop::GetActive())
		{
			return eventLoop->Pending();
		}
		return false;
	}
	bool CoreApplication::Dispatch()
	{
		if (IEventLoop* eventLoop = IEventLoop::GetActive())
		{
			return eventLoop->Dispatch();
		}
		return false;
	}
	bool CoreApplication::DispatchIdle()
	{
		// Synthesize an idle event and check if more of them are needed
		IdleEvent& event = BuildProcessEvent(IdleEvent::EvtIdle).SetSourceToSelf().Execute().GetEvent();

		// Flush the logged messages if any (do this after processing the events which could have logged new messages)
		wxLog::FlushActive();

		// Garbage collect all objects previously scheduled for destruction
		FinalizeScheduledForDestruction();

		return event.IsMoreRequested();
	}
	bool CoreApplication::Yield(FlagSet<EventYieldFlag> flags)
	{
		if (IEventLoop* eventLoop = IEventLoop::GetActive())
		{
			return eventLoop->Yield(flags);
		}
		else
		{
			// TODO: Create temporary event loop
		}
		return false;
	}

	// Application::IPendingEvents
	bool CoreApplication::IsScheduledForDestruction(const wxObject& object) const
	{
		ReadLockGuard lock(m_ScheduledForDestructionLock);

		return Utility::Contains(m_ScheduledForDestruction, [&](const auto& item)
		{
			return item.get() == &object;
		});
	}
	void CoreApplication::ScheduleForDestruction(std::unique_ptr<wxObject> object)
	{
		// We need an active event loop to schedule deletion
		if (IEventLoop::GetActive() && object)
		{
			WriteLockGuard lock(m_ScheduledForDestructionLock);

			if (!Utility::Contains(m_ScheduledForDestruction, [&](const auto& item)
			{
				return item.get() == object.get();
			}))
			{
				m_ScheduledForDestruction.emplace_back(std::move(object));
			}
		}
	}
	void CoreApplication::FinalizeScheduledForDestruction()
	{
		WriteLockGuard lock(m_ScheduledForDestructionLock);
		m_ScheduledForDestruction.clear();
	}

	void CoreApplication::AddPendingEventHandler(EvtHandler& evtHandler)
	{
		WriteLockGuard lock(m_PendingEvtHandlersLock);

		if (!Utility::Contains(m_PendingEvtHandlers, [&](const EvtHandler* item)
		{
			return item == &evtHandler;
		}))
		{
			m_PendingEvtHandlers.emplace_back(&evtHandler);
		}
	}
	bool CoreApplication::RemovePendingEventHandler(EvtHandler& evtHandler)
	{
		size_t count = 0;
		auto Find = [&](const EvtHandler* item)
		{
			if (item == &evtHandler)
			{
				count++;
				return true;
			}
			return false;
		};

		// Try to remove the handler from both lists 
		if (WriteLockGuard lock(m_PendingEvtHandlersLock); true)
		{
			Utility::RemoveSingleIf(m_PendingEvtHandlers, Find);
			Utility::RemoveSingleIf(m_DelayedPendingEvtHandlers, Find);
		}
		return count != 0;
	}
	void CoreApplication::DelayPendingEventHandler(EvtHandler& evtHandler)
	{
		// Move the handler from the list of handlers with processable pending events
		// to the list of handlers with pending events which needs to be processed later.
		auto Find = [&](const EvtHandler* item)
		{
			return item == &evtHandler;
		};

		if (WriteLockGuard lock(m_PendingEvtHandlersLock); true)
		{
			Utility::RemoveSingleIf(m_PendingEvtHandlers, Find);
			if (!Utility::Contains(m_DelayedPendingEvtHandlers, Find))
			{
				m_DelayedPendingEvtHandlers.emplace_back(&evtHandler);
			}
		}
	}

	bool CoreApplication::ProcessPendingEvents()
	{
		if (m_PendingEventsProcessingEnabled)
		{
			WriteLockGuard lock(m_PendingEvtHandlersLock);

			// This helper list should be empty
			if (!m_DelayedPendingEvtHandlers.empty())
			{
				return false;
			}

			// Iterate until the list becomes empty: the handlers remove themselves from it when they don't have any more pending events
			size_t count = 0;
			while (!m_PendingEvtHandlers.empty())
			{
				// In 'EvtHandler::ProcessPendingEvents', new handlers might be added and we are required to unlock the lock here.
				EvtHandler* evtHandler = m_PendingEvtHandlers.front();
				
				// This inner unlock-relock must be consistent with the outer guard.
				m_PendingEvtHandlersLock.UnlockWrite();
				Utility::CallAtScopeExit relock = [&]()
				{
					m_PendingEvtHandlersLock.LockWrite();
				};

				// We always call 'EvtHandler::ProcessPendingEvents' on the first event handler with pending events because handlers
				// auto-remove themselves from this list (see 'RemovePendingEventHandler') if they have no more pending events.
				if (evtHandler->ProcessPendingEvents())
				{
					count++;
				}
			}

			// Now the 'm_PendingEvtHandlers' is surely empty, however some event handlers may have moved themselves into
			// 'm_DelayedPendingEvtHandlers' because of a selective 'Yield' call in progress. Now we need to move them back
			// to 'm_PendingEvtHandlersLock' so the next call to this function has the chance of processing them.
			if (!m_DelayedPendingEvtHandlers.empty())
			{
				m_PendingEvtHandlers.insert(m_PendingEvtHandlers.end(), m_DelayedPendingEvtHandlers.begin(), m_DelayedPendingEvtHandlers.end());
				m_DelayedPendingEvtHandlers.clear();
			}
			return count != 0;
		}
		return false;
	}
	size_t CoreApplication::DiscardPendingEvents()
	{
		WriteLockGuard lock(m_PendingEvtHandlersLock);

		size_t count = 0;
		for (EvtHandler* evtHandler: m_PendingEvtHandlers)
		{
			if (evtHandler->DiscardPendingEvents() != 0)
			{
				count++;
			}
		}
		m_PendingEvtHandlers.clear();

		return count;
	}

	// Application::IExceptionHandler
	bool CoreApplication::OnMainLoopException()
	{
		return m_NativeApp.wxApp::OnExceptionInMainLoop();
	}
	void CoreApplication::OnFatalException()
	{
		m_NativeApp.wxApp::OnFatalException();
	}
	void CoreApplication::OnUnhandledException()
	{
		m_NativeApp.wxApp::OnUnhandledException();
	}

	bool CoreApplication::StoreCurrentException()
	{
		if (!m_StoredException)
		{
			m_StoredException = std::current_exception();
			return true;
		}
		else
		{
			// We can't store more than one exception currently: while we could  support this by just
			// using a 'std::vector<std::exception_ptr>', it shouldn't be actually necessary because
			// we should never have more than one active exception anyhow.
			return false;
		}
	}
	void CoreApplication::RethrowStoredException()
	{
		if (std::exception_ptr storedException = std::move(m_StoredException))
		{
			std::rethrow_exception(storedException);
		}
	}

	// Application::IDebugHandler
	void CoreApplication::OnAssertFailure(String file, int line, String function, String condition, String message)
	{
		m_NativeApp.wxApp::OnAssertFailure(file.wx_str(), line, function.wx_str(), condition.wx_str(), message.wx_str());
	}

	// Application::ICommandLine
	size_t CoreApplication::EnumCommandLineArgs(std::function<bool(String)> func) const
	{
		auto DoEnum = [&](auto&& argv, size_t argc)
		{
			size_t count = 0;
			for (size_t i = 0; i < argc; i++)
			{
				count++;
				if (!std::invoke(func, argv[i]))
				{
					break;
				}
			}
			return count;
		};

		if (m_ArgVW)
		{
			return DoEnum(m_ArgVW, m_ArgC);
		}
		else if (m_ArgVA)
		{
			return DoEnum(m_ArgVA, m_ArgC);
		}
		return 0;
	}
	void CoreApplication::OnCommandLineInit(wxCmdLineParser& parser)
	{
		m_NativeApp.wxApp::OnInitCmdLine(parser);
	}
	bool CoreApplication::OnCommandLineParsed(wxCmdLineParser& parser)
	{
		return m_NativeApp.wxApp::OnCmdLineParsed(parser);
	}
	bool CoreApplication::OnCommandLineError(wxCmdLineParser& parser)
	{
		return m_NativeApp.wxApp::OnCmdLineError(parser);
	}
	bool CoreApplication::OnCommandLineHelp(wxCmdLineParser& parser)
	{
		return m_NativeApp.wxApp::OnCmdLineHelp(parser);
	}
}
