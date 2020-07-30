#pragma once
#include "Common.h"
#include "IEvtHandler.h"
#include "kxf/Threading/LockGuard.h"
#include "kxf/Threading/ReadWriteLock.h"
#include "kxf/Threading/RecursiveRWLock.h"

namespace kxf
{
	class IEventExecutor;
	class EvtHandlerDelegate;
}

namespace kxf
{
	class KX_API EvtHandler: public RTTI::ImplementInterface<EvtHandler, IEvtHandler>
	{
		private:
			struct PendingItem final
			{
				std::unique_ptr<IEvent> Event;
				FlagSet<ProcessEventFlag> ProcessFlags;
			};

		private:
			// Dynamic events table
			RecursiveRWLock m_EventTableLock;
			std::vector<EventItem> m_EventTable;
			size_t m_EventBindSlot = 0;

			// Pending events
			ReadWriteLock m_PendingEventsLock;
			std::vector<PendingItem> m_PendingEvents;

			// Events chain
			std::atomic<IEvtHandler*> m_PrevHandler = nullptr;
			std::atomic<IEvtHandler*> m_NextHandler = nullptr;

			// Enabled/disabled switch
			std::atomic<bool> m_IsEnabled = true;

		private:
			void Move(EvtHandler&& other, bool destroy);
			void Destroy();

		protected:
			void PrepareEvent(IEvent& event, const EventID& eventID, const UniversallyUniqueID& uuid = {});
			bool FreeBindSlot(const LocallyUniqueID& bindSlot);
			void FreeAllBindSlots();

			bool TryApp(IEvent& event);
			bool TryChain(IEvent& event);
			bool TryLocally(IEvent& event);
			bool TryHereOnly(IEvent& event);
			bool TryBeforeAndHere(IEvent& event);

			bool SearchEventTable(IEvent& event);
			bool ExecuteEventHandler(IEvent& event, EventItem& eventItem, IEvtHandler& evtHandler, bool& wasQueued);
			void ConsumeException(IEvent& event);

		protected:
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<EventFlag> flags = {}) override;
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor) override;
			bool DoUnbind(const LocallyUniqueID& bindSlot) override;

			bool OnDynamicBind(EventItem& eventItem) override
			{
				return true;
			}
			bool OnDynamicUnbind(EventItem& eventItem) override
			{
				return true;
			}

			void DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}) override;
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}, IEvtHandler* onlyIn = nullptr) override;

			bool TryBefore(IEvent& event) override;
			bool TryAfter(IEvent& event) override;

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
			bool ProcessPendingEvents() override;
			size_t DiscardPendingEvents() override;

			// Event handlers chain
			IEvtHandler* GetPrevHandler() const override
			{
				return m_PrevHandler;
			}
			IEvtHandler* GetNextHandler() const override
			{
				return m_NextHandler;
			}
			void SetPrevHandler(IEvtHandler* evtHandler) override
			{
				m_PrevHandler = evtHandler;
			}
			void SetNextHandler(IEvtHandler* evtHandler) override
			{
				m_NextHandler = evtHandler;
			}

			void Unlink() override;
			bool IsUnlinked() const override;

			bool IsEventProcessingEnabled() const override
			{
				return m_IsEnabled;
			}
			void EnableEventProcessing(bool enable = true) override
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
