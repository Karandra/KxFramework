#pragma once
#include "Common.h"
#include "EvtHandler.h"
#include "kxf/Utility/WithOptionalOwnership.h"

namespace kxf
{
	class KX_API EvtHandlerDelegate:
		public EventSystem::BindCRTP<EvtHandlerDelegate>,
		public EventSystem::CallAfterCRTP<EvtHandlerDelegate>,
		public EventSystem::QueueEventCRTP<EvtHandlerDelegate>,
		public EventSystem::ProcessEventCRTP<EvtHandlerDelegate>
	{
		protected:
			using EventItem = EventSystem::EventItem;

		private:
			Utility::WithOptionalOwnership<EvtHandler> m_EvtHandler;

		protected:
			void DoQueueEvent(std::unique_ptr<Event> event, const EventID& eventID = {}, UniversallyUniqueID uuid = {})
			{
				m_EvtHandler->DoQueueEvent(std::move(event), eventID, std::move(uuid));
			}
			bool DoProcessEvent(Event& event, const EventID& eventID = {}, EvtHandler* onlyIn = nullptr)
			{
				return m_EvtHandler->DoProcessEvent(event, eventID, onlyIn);
			}
			bool DoProcessEventSafely(Event& event, const EventID& eventID = {})
			{
				return m_EvtHandler->DoProcessEventSafely(event, eventID);
			}
			bool DoProcessEventLocally(Event& event, const EventID& eventID = {})
			{
				return m_EvtHandler->DoProcessEventLocally(event, eventID);
			}

			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<EventFlag> flags = {})
			{
				return m_EvtHandler->DoBind(eventID, std::move(executor), flags);
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor)
			{
				return m_EvtHandler->DoUnbind(eventID, executor);
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot)
			{
				return m_EvtHandler->DoUnbind(bindSlot);
			}

		public:
			EvtHandlerDelegate(EvtHandler& evtHandler) noexcept
			{
				m_EvtHandler.Assign(evtHandler);
			}
			EvtHandlerDelegate(std::unique_ptr<EvtHandler> evtHandler) noexcept
			{
				m_EvtHandler.Assign(std::move(evtHandler));
			}
			virtual ~EvtHandlerDelegate() = default;

		public:
			// Event queuing and processing
			bool ProcessPendingEvents()
			{
				return m_EvtHandler->ProcessPendingEvents();
			}
			size_t DiscardPendingEvents()
			{
				return m_EvtHandler->DiscardPendingEvents();
			}

			// Event handlers chain
			EvtHandler* GetPrevHandler() const
			{
				return m_EvtHandler->GetPrevHandler();
			}
			EvtHandler* GetNextHandler() const
			{
				return m_EvtHandler->GetNextHandler();
			}
			void SetPrevHandler(EvtHandler* evtHandler)
			{
				m_EvtHandler->SetPrevHandler(evtHandler);
			}
			void SetNextHandler(EvtHandler* evtHandler)
			{
				m_EvtHandler->SetNextHandler(evtHandler);
			}

			void Unlink()
			{
				m_EvtHandler->Unlink();
			}
			bool IsUnlinked() const
			{
				return m_EvtHandler->IsUnlinked();
			}

			bool IsEventProcessingEnabled() const
			{
				return m_EvtHandler->IsEventProcessingEnabled();
			}
			void EnableEventProcessing(bool enable = true)
			{
				m_EvtHandler->EnableEventProcessing(enable);
			}
	};
}
