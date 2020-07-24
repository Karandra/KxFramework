#include "stdafx.h"
#include "EvtHandler.h"
#include "IdleEvent.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/Application/Private/Utility.h"
#include "kxf/Utility/Container.h"

namespace kxf
{
	void EvtHandler::Move(EvtHandler&& other, bool destroy)
	{
		if (this == &other)
		{
			return;
		}
		if (destroy)
		{
			Destroy();
		}

		m_EventTable = std::move(other.m_EventTable);
		{
			WriteLockGuard lockThis(m_EventTableLock);
			WriteLockGuard lockOther(other.m_EventTableLock);
			m_EventTable = std::move(other.m_EventTable);
		}
		{
			WriteLockGuard lockThis(m_PendingEventsLock);
			WriteLockGuard lockOther(other.m_PendingEventsLock);
			m_PendingEvents = std::move(other.m_PendingEvents);
		}
		m_PrevHandler.exchange(other.m_PrevHandler);
		m_NextHandler.exchange(other.m_NextHandler);
		m_IsEnabled.exchange(other.m_IsEnabled);
	}
	void EvtHandler::Destroy()
	{
		Unlink();

		// Remove us from the global list of the pending events if necessary
		if (auto app = ICoreApplication::GetInstance())
		{
			app->RemovePendingEventHandler(*this);
		}
		DiscardPendingEvents();
	}

	void EvtHandler::PrepareEvent(IEvent& event, const EventID& eventID, UniversallyUniqueID uuid)
	{
		event.OnStartProcess(eventID, uuid);
	}
	bool EvtHandler::FreeBindSlot(const LocallyUniqueID& bindSlot)
	{
		// If it's the last bind slot we have used, free it.
		if (m_EventBindSlot == bindSlot.ToInt())
		{
			--m_EventBindSlot;
			return true;
		}
		return false;
	}
	void EvtHandler::FreeAllBindSlots()
	{
		m_EventBindSlot = 0;
	}

	bool EvtHandler::TryApp(IEvent& event)
	{
		auto app = ICoreApplication::GetInstance();
		if (app && this != app)
		{
			// Special case: don't pass 'IIdleEvent::EvtIdle' to app, since it'll always
			// swallow it. Events of 'IIdleEvent::EvtIdle' are sent explicitly to wxApp so
			// it will be processed appropriately via 'EvtHandler::SearchEventTable'.
			if (event.GetEventID() != IdleEvent::EvtIdle && app->DoProcessEvent(event))
			{
				return true;
			}
		}
		return false;
	}
	bool EvtHandler::TryChain(IEvent& event)
	{
		for (EvtHandler* evtHandler = m_NextHandler; evtHandler; evtHandler = evtHandler->m_NextHandler)
		{
			// We need to process this event at the level of this handler only
			// right now, the pre-/post-processing was either already done by
			// 'DoProcessEvent' from which we were called or will be done by it when
			// we return.
			//
			// However we must call 'DoProcessEvent' and not 'TryHereOnly' because the
			// existing code (including some in wxWidgets itself) expects the
			// overridden ProcessEvent() in its custom event handlers pushed on a
			// window to be called. 
			//
			// So we must call 'DoProcessEvent' but it must not do what it usually
			// does. To resolve this paradox we set up a special parameter for 'DoProcessEvent'
			// to make it process only in specified handler.

			if (evtHandler->DoProcessEvent(event, {}, evtHandler))
			{
				// Make sure "skipped" flag is not set as the event was really
				// processed in this case. Normally it shouldn't be set anyhow but
				// make sure just in case the user code does something strange.

				event.Skip(false);
				return true;
			}
		}
		return false;
	}
	bool EvtHandler::TryLocally(IEvent& event)
	{
		// Try the hooks which should be called before our own handlers and this
		// handler itself first. Notice that we should not call 'DoProcessEvent' on
		// this one as we're already called from it, which explains why we do it
		// here and not in TryChain()
		return TryBeforeAndHere(event) || TryChain(event);
	}
	bool EvtHandler::TryHereOnly(IEvent& event)
	{
		// If the event handler is disabled it doesn't process any events
		if (m_IsEnabled)
		{
			// There is an implicit entry for async method calls processing in every event handler
			IAsyncEvent* asyncEvent = nullptr;
			if (event.GetEventSource() == this && event.QueryInterface(asyncEvent))
			{
				asyncEvent->Execute();
				return true;
			}

			// Handle dynamic event table
			if (SearchEventTable(event))
			{
				return true;
			}
		}

		// We don't have a handler for this event
		return false;
	}
	bool EvtHandler::TryBeforeAndHere(IEvent& event)
	{
		// Another helper which simply calls pre-processing hook and then tries to handle the event at this handler level
		return TryBefore(event) || TryHereOnly(event);
	}

	bool EvtHandler::SearchEventTable(IEvent& event)
	{
		size_t initialSize = 0;
		size_t nullCount = 0;

		if (ReadLockGuard lock(m_EventTableLock); !m_EventTable.empty())
		{
			initialSize = m_EventTable.size();
			for (auto it = m_EventTable.rbegin(); it != m_EventTable.rend(); ++it)
			{
				EventItem& eventItem = *it;
				if (eventItem.IsNull())
				{
					// This entry must have been unbound at some time in the past, so
					// skip it now and really remove it from the event table, once we
					// finish iterating.
					nullCount++;
					continue;
				}

				if (eventItem.GetEventID() == event.GetEventID())
				{
					EvtHandler* evtHandler = eventItem.GetExecutor()->GetTargetHandler();
					if (!evtHandler)
					{
						evtHandler = this;
					}
					if (ExecuteDirectEvent(event, eventItem, *evtHandler))
					{
						// It's important to skip clearing of the unbound event entries
						// below because this object itself could have been deleted by
						// the event handler making m_EventTable a dangling pointer
						// which can't be accessed any longer in the code below.
						//
						// In practice, it hopefully shouldn't be a problem to wait
						// until we get an event that we don't handle before clearing
						// because this should happen soon enough and even if it
						// doesn't the worst possible outcome is slightly increased
						// memory consumption while not skipping clearing can result in
						// hard to reproduce crashes (because it requires the disconnection
						// and deletion happen at the same time which is not always the
						// case).
						return true;
					}
				}
			}
		}

		if (nullCount != 0)
		{
			WriteLockGuard lock(m_EventTableLock);
			Utility::RemoveAllIf(m_EventTable, [](const EventItem& item)
			{
				return item.IsNull();
			});

			// Shrink vector only if we have deleted enough items to justify reallocation
			if (initialSize / 2 >= nullCount)
			{
				m_EventTable.shrink_to_fit();
			}
		}
		return false;
	}
	bool EvtHandler::ExecuteDirectEvent(IEvent& event, EventItem& eventItem, EvtHandler& evtHandler)
	{
		// Reset skip instruction
		event.Skip(false);

		// Call the handler
		ExecuteEventHandler(event, *eventItem.GetExecutor(), evtHandler);

		// Return true if we processed this event and event handler itself didn't skipped it
		return !event.IsSkipped();
	}
	void EvtHandler::ExecuteEventHandler(IEvent& event, IEventExecutor& executor, EvtHandler& evtHandler)
	{
		executor.Execute(evtHandler, event);

		// If event wasn't skipped call callback for this event and restore any possible changes in skip state
		if (const bool isSkipped = event.IsSkipped(); !isSkipped)
		{
			//event.ExecuteCallback(evtHandler);
			//event.Skip(isSkipped);
		}
	}
	
	void EvtHandler::DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID, UniversallyUniqueID uuid)
	{
		auto app = ICoreApplication::GetInstance();
		if (app && event)
		{
			PrepareEvent(*event, eventID, std::move(uuid));

			if (WriteLockGuard lock(m_PendingEventsLock); true)
			{
				// Add this event to our list of pending events
				if (uuid)
				{
					// If this event has a unique ID, search for the last posted event with the same ID and,
					// if it'll be found, replace it with our new event, otherwise just add it at the end as usual.
					auto it = std::find_if(m_PendingEvents.rbegin(), m_PendingEvents.rend(), [&](const auto& event)
					{
						return event->GetUniqueID() == uuid;
					});
					if (it != m_PendingEvents.rend())
					{
						*it = std::move(event);
					}
					else
					{
						m_PendingEvents.emplace_back(std::move(event));
					}
				}
				else
				{
					m_PendingEvents.emplace_back(std::move(event));
				}

				// Add this event handler to the list of event handlers that have pending events
				app->AddPendingEventHandler(*this);

				// Only release 'm_PendingEventsLock' now because otherwise there is a race condition: we could process
				// the event just added to m_pendingEvents in our 'ProcessPendingEvents' before we had time to append
				// this pointer to application's list; thus breaking the invariant that a handler should be in the list
				// if and only if it has any pending events to process.
			}

			// Inform the system that new pending events are somewhere, and that these should be processed in idle time.
			app->WakeUp();
		}
	}
	bool EvtHandler::DoProcessEvent(IEvent& event, const EventID& eventID, EvtHandler* onlyIn)
	{
		PrepareEvent(event, eventID);

		// The very first thing we do is to allow any registered filters to hook
		// into event processing in order to globally pre-process all events.
		//
		// Note that we should only do it if we're the first event handler called
		// to avoid calling 'FilterEvent' multiple times as the event goes through
		// the event handler chain and possibly upwards the window hierarchy.
		if (!event.WasProcessed())
		{
			using Result = IEventFilter::Result;
			if (auto app = ICoreApplication::GetInstance())
			{
				const Result result = app->FilterEvent(event);
				if (result != Result::Skip)
				{
					return result != Result::Ignore;
				}
				// Proceed normally
			}
		}

		// Short circuit the event processing logic if we're requested to process
		// this event in this handler only, see 'TryChain' for more details.
		if (this == onlyIn)
		{
			return TryBeforeAndHere(event);
		}

		// Try to process the event in this handler itself
		if (TryLocally(event))
		{
			// It is possible that 'TryChain' called from 'ProcessEventLocally'
			// returned true but the event was not really processed: this happens
			// if a custom handler ignores the request to process the event in this
			// handler only and in this case we should skip the post processing
			// done in 'TryAfter' but still return the correct value ourselves to
			// indicate whether we did or did not find a handler for this event.
			return !event.IsSkipped();
		}

		// If we still didn't find a handler, propagate the event upwards the
		// window chain and/or to the application object.
		if (TryAfter(event))
		{
			return true;
		}

		// No handler found anywhere, bail out.
		return false;
	}

	bool EvtHandler::TryBefore(IEvent& event)
	{
		return false;
	}
	bool EvtHandler::TryAfter(IEvent& event)
	{
		// We only want to pass the window to the application object once even if
		// there are several chained handlers. Ensure that this is what happens by
		// only calling DoTryApp() if there is no next handler (which would do it).
		//
		// Notice that, unlike simply calling TryAfter() on the last handler in the
		// chain only from ProcessEvent(), this also works with wxWindow object in
		// the middle of the chain: its overridden TryAfter() will still be called
		// and propagate the event upwards the window hierarchy even if it's not
		// the last one in the chain (which, admittedly, shouldn't happen often).
		if (EvtHandler* next = m_NextHandler)
		{
			return next->TryAfter(event);
		}

		// If this event is going to be processed in another handler next, don't pass it
		// to application instance now, it will be done from 'TryAfter' of this other handler.
		if (event.WillBeProcessedAgain())
		{
			return false;
		}
		return TryApp(event);
	}

	bool EvtHandler::DoProcessEventSafely(IEvent& event, const EventID& eventID)
	{
		try
		{
			return DoProcessEvent(event, eventID);
		}
		catch (...)
		{
			ConsumeException(event);
			return false;
		}
	}
	bool EvtHandler::DoProcessEventLocally(IEvent& event, const EventID& eventID)
	{
		PrepareEvent(event, eventID);
		return TryLocally(event);
	}
	void EvtHandler::ConsumeException(IEvent& event)
	{
		auto app = ICoreApplication::GetInstance();
		try
		{
			if (!app || !app->OnMainLoopException())
			{
				// If OnExceptionInMainLoop() returns false, we're supposed to exit
				// the program and for this we need to exit the main loop, not the
				// possibly nested one we're running right now.
				if (app)
				{
					app->ExitMainLoop();
				}
				else
				{
					// We must not continue running after an exception, unless  explicitly requested,
					// so if we can't ensure this in any other way, do it brutally like this.
					std::terminate();
				}

			}
			// Continue running current event loop
		}
		catch (...)
		{
			// 'OnMainLoopException' threw, possibly rethrowing the same exception again. 
			// We have to deal with it here because we can't allow the exception to escape
			// from the handling code, this will result in a crash at best and in something
			// even more weird at worst (like exceptions completely disappearing into the void
			// under some 64-bit versions of Windows).
			IEventLoop* eventLoop = app->GetActiveEventLoop();
			if (eventLoop && eventLoop->IsYielding())
			{
				eventLoop->Exit();
			}

			// Give the application one last possibility to store the exception for rethrowing it later, when we get back to our code.
			bool exceptionStored = false;
			try
			{
				if (app)
				{
					exceptionStored = app->StoreCurrentException();
				}
			}
			catch (...)
			{
				// 'StoreCurrentException' really shouldn't throw, but if it did, take it as an indication that it didn't store it.
			}

			// If it didn't take it, just abort, at least like this we behave consistently everywhere.
			if (!exceptionStored)
			{
				try
				{
					if (app)
					{
						app->OnUnhandledException();
					}
				}
				catch (...)
				{
					// And 'OnUnhandledException' absolutely shouldn't throw, but we still must account for the possibility that it
					// did. At least show some information about the exception in this case.
					Application::Private::OnUnhandledException();
				}
				std::terminate();
			}
		}
	}

	LocallyUniqueID EvtHandler::DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<EventFlag> flags)
	{
		if (executor && eventID && eventID != IEvent::EvtAny && eventID != wxEVT_ASYNC_METHOD_CALL)
		{
			EventItem eventItem(eventID, std::move(executor));
			eventItem.SetFlags(flags);

			if (OnDynamicBind(eventItem) && eventItem)
			{
				WriteLockGuard lock(m_EventTableLock);

				// Test for the bind slots count, we can't allow this to overflow.
				const size_t nextBindSlot = m_EventBindSlot + 1;
				if (nextBindSlot == std::numeric_limits<size_t>::max())
				{
					throw std::runtime_error("exceeded maximum number of bind slots [std::numeric_limits<size_t>::max()]");
				}

				m_EventBindSlot = nextBindSlot;
				m_EventTable.emplace_back(std::move(eventItem)).SetBindSlot(nextBindSlot);
				return nextBindSlot;
			}
		}
		return {};
	}
	bool EvtHandler::DoUnbind(const EventID& eventID, IEventExecutor& executor)
	{
		if (eventID)
		{
			if (WriteLockGuard lock(m_EventTableLock); !m_EventTable.empty())
			{
				EventItem dummyItem(eventID, executor);
				IEventExecutor& nullExecutor = EventSystem::NullEventExecutor::Get();

				size_t unbindCount = 0;
				for (auto it = m_EventTable.rbegin(); it != m_EventTable.rend(); ++it)
				{
					// 1) If 'eventID' is 'IEvent::EvtAny' unbind *all* events.
					// 2) If 'eventID' matches our item ID and the supplied executor is a special null executor unbind all events with this ID.
					// 3) Unbind the event if both event ID and the executor matches (See 'EventItem::IsSameAs' for details).
					if ((eventID == IEvent::EvtAny) || (eventID == it->GetEventID() && &executor == &nullExecutor) || (it->IsSameAs(dummyItem)))
					{
						// Clear the handler if derived class allows that
						if (OnDynamicUnbind(*it))
						{
							// We can't delete the entry from the vector if we're currently iterating over it.
							// As we don't know whether we're or not, just null it for now and we will really
							// erase it when we do finish iterating over it the next time.
							FreeBindSlot(it->GetBindSlot());
							*it = {};

							unbindCount++;
						}
					}
				}

				// If we were asked to unbind all items (or we've deleted them all) we can free all bind slots as well
				if (eventID == IEvent::EvtAny || unbindCount == m_EventTable.size())
				{
					FreeAllBindSlots();
				}
				return unbindCount != 0;
			}
		}
		return false;
	}
	bool EvtHandler::DoUnbind(const LocallyUniqueID& bindSlot)
	{
		if (bindSlot)
		{
			if (WriteLockGuard lock(m_EventTableLock); !m_EventTable.empty())
			{
				size_t unbindCount = 0;
				auto it = std::find_if(m_EventTable.rbegin(), m_EventTable.rend(), [&](const EventItem& item)
				{
					return item.GetBindSlot() == bindSlot;
				});
				if (it != m_EventTable.rend() && OnDynamicUnbind(*it))
				{
					// See the comment in the 'DoUnbind(EventID, IEventExecutor&)' overload above for why
					// we just null the event item and not erase it.
					FreeBindSlot(bindSlot);
					*it = {};

					unbindCount++;
					return true;
				}

				if (unbindCount == m_EventTable.size())
				{
					FreeAllBindSlots();
				}
				return unbindCount != 0;
			}
		}
		return false;
	}

	bool EvtHandler::ProcessPendingEvents()
	{
		// We need an event loop which manages the list of event handlers with pending events. Cannot proceed without it!
		if (auto app = ICoreApplication::GetInstance())
		{
			// We need to process only a single pending event in this call because each call to 'DoProcessEvent'
			// could result in the destruction of this same event handler (see the comment at the end of this function).
			std::unique_ptr<IEvent> event;

			// This method is only called by an application if this handler does have pending events
			if (WriteLockGuard lock(m_PendingEventsLock); !m_PendingEvents.empty())
			{
				// Always get the first event
				auto eventIt = m_PendingEvents.begin();

				// If we're inside 'Yield' call, process events selectively instead
				IEventLoop* eventLoop = app->GetActiveEventLoop();
				if (eventLoop && eventLoop->IsYielding())
				{
					// Find the first event which can be processed now
					auto it = m_PendingEvents.begin();
					for (; it != m_PendingEvents.end(); ++it)
					{
						if (eventLoop->IsEventAllowedInsideYield((*it)->GetEventCategory()))
						{
							eventIt = std::move(it);
							break;
						}
					}

					if (it == m_PendingEvents.end())
					{
						// All our events are *not* processable now, signal this.
						app->DelayPendingEventHandler(*this);

						// See the comment at the beginning of 'IEventLoop' header for the logic behind YieldFor() and behind DelayPendingEventHandler().
						return false;
					}
				}

				// It's important we remove event from list before processing it, else a nested event loop,
				// for example from a modal dialog, might process the same event again.
				event = std::move(*eventIt);
				m_PendingEvents.erase(eventIt);

				if (m_PendingEvents.empty())
				{
					// If there are no more pending events left, we don't need to stay in this list.
					app->RemovePendingEventHandler(*this);
				}
			}
			
			if (event)
			{
				// Careful: this object could have been deleted by the event handler executed by the 'DoProcessEvent' call below,
				// so we can't access any fields of this object anymore.
				DoProcessEvent(*event);

				// Should we return the result of 'DoProcessEvent' here?
				return true;
			}
		}
		return false;
	}
	size_t EvtHandler::DiscardPendingEvents()
	{
		WriteLockGuard lock(m_PendingEventsLock);

		const size_t count = m_PendingEvents.size();
		m_PendingEvents.clear();
		return count;
	}

	void EvtHandler::Unlink()
	{
		// This event handler must take itself out of the chain of handlers
		EvtHandler* prev = m_PrevHandler;
		EvtHandler* next = m_NextHandler;
		if (prev)
		{
			prev->SetNextHandler(next);
		}
		if (next)
		{
			next->SetPrevHandler(prev);
		}

		m_PrevHandler = nullptr;
		m_NextHandler = nullptr;
	}
	bool EvtHandler::IsUnlinked() const
	{
		return m_PrevHandler == nullptr && m_NextHandler == nullptr;
	}
}
