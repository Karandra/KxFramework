#pragma once
#include "Common.h"
#include "Event.h"
#include "EventID.h"
#include "EventItem.h"
#include "CRTP/Bind.h"
#include "CRTP/CallAfter.h"
#include "CRTP/QueueEvent.h"
#include "CRTP/ProcessEvent.h"
#include "kxf/RTTI/QueryInterface.h"
#include "kxf/Threading/LockGuard.h"
#include "kxf/Threading/ReadWriteLock.h"
#include "kxf/Threading/RecursiveRWLock.h"
#include <atomic>

namespace kxf
{
	class IEventExecutor;
	class EvtHandlerDelegate;
}

namespace kxf
{
	class KX_API EvtHandler:
		public RTTI::Interface<EvtHandler>,
		public EventSystem::BindCRTP<EvtHandler>,
		public EventSystem::CallAfterCRTP<EvtHandler>,
		public EventSystem::QueueEventCRTP<EvtHandler>,
		public EventSystem::ProcessEventCRTP<EvtHandler>
	{
		KxDeclareIID(EvtHandler, {0x96ae0970, 0x8cc5, 0x4288, {0xb1, 0x1b, 0x7a, 0xe6, 0x42, 0xf8, 0xdf, 0x8c}});

		friend class CoreApplication;
		friend class EvtHandlerDelegate;
		friend class EventSystem::BindCRTP<EvtHandler>;
		friend class EventSystem::CallAfterCRTP<EvtHandler>;
		friend class EventSystem::QueueEventCRTP<EvtHandler>;
		friend class EventSystem::ProcessEventCRTP<EvtHandler>;

		protected:
			using EventItem = EventSystem::EventItem;

		private:
			// Dynamic events table
			RecursiveRWLock m_EventTableLock;
			std::vector<EventItem> m_EventTable;
			size_t m_EventBindSlot = 0;

			// Pending events
			ReadWriteLock m_PendingEventsLock;
			std::vector<std::unique_ptr<Event>> m_PendingEvents;

			// Events chain
			std::atomic<EvtHandler*> m_PrevHandler = nullptr;
			std::atomic<EvtHandler*> m_NextHandler = nullptr;

			// Enabled/disabled switch
			std::atomic<bool> m_IsEnabled = true;

		protected:
			void Move(EvtHandler&& other, bool destroy);
			void Destroy();

			void PrepareEvent(Event& event, const EventID& eventID, UniversallyUniqueID uuid = {});
			bool FreeBindSlot(const LocallyUniqueID& bindSlot);
			void FreeAllBindSlots();

			bool TryApp(Event& event);
			bool TryChain(Event& event);
			bool TryLocally(Event& event);
			bool TryHereOnly(Event& event);
			bool TryBeforeAndHere(Event& event);

			bool SearchEventTable(Event& event);
			bool ExecuteDirectEvent(Event& event, EventItem& eventItem, EvtHandler& evtHandler);
			void ExecuteEventHandler(Event& event, IEventExecutor& executor, EvtHandler& evtHandler);
			
			bool DoProcessEventSafely(Event& event, const EventID& eventID = {});
			bool DoProcessEventLocally(Event& event, const EventID& eventID = {});
			void ConsumeException(Event& event);

		protected:
			virtual LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<EventFlag> flags = {});
			virtual bool DoUnbind(const EventID& eventID, IEventExecutor& executor);
			virtual bool DoUnbind(const LocallyUniqueID& bindSlot);

			virtual bool OnDynamicBind(EventItem& eventItem)
			{
				return true;
			}
			virtual bool OnDynamicUnbind(EventItem& eventItem)
			{
				return true;
			}

			virtual void DoQueueEvent(std::unique_ptr<Event> event, const EventID& eventID = {}, UniversallyUniqueID uuid = {});
			virtual bool DoProcessEvent(Event& event, const EventID& eventID = {}, EvtHandler* onlyIn = nullptr);

			virtual bool TryBefore(Event& event);
			virtual bool TryAfter(Event& event);

		public:
			EvtHandler() = default;
			EvtHandler(const EvtHandler&) = delete;
			EvtHandler(EvtHandler&& other) noexcept
			{
				Move(std::move(other), false);
			}
			~EvtHandler()
			{
				Destroy();
			}

		public:
			// Event queuing and processing
			bool ProcessPendingEvents();
			size_t DiscardPendingEvents();

			// Event handlers chain
			EvtHandler* GetPrevHandler() const
			{
				return m_PrevHandler;
			}
			EvtHandler* GetNextHandler() const
			{
				return m_NextHandler;
			}
			virtual void SetPrevHandler(EvtHandler* evtHandler)
			{
				m_PrevHandler = evtHandler;
			}
			virtual void SetNextHandler(EvtHandler* evtHandler)
			{
				m_NextHandler = evtHandler;
			}

			void Unlink();
			bool IsUnlinked() const;

			bool IsEventProcessingEnabled() const
			{
				return m_IsEnabled;
			}
			void EnableEventProcessing(bool enable = true)
			{
				m_IsEnabled = enable;
			}

		public:
			EvtHandler& operator=(const EvtHandler&) = delete;
			EvtHandler& operator=(EvtHandler&& other) noexcept
			{
				Move(std::move(other), true);
				return *this;
			}
	};
}
