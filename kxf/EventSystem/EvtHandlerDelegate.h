#pragma once
#include "Common.h"
#include "IEvtHandler.h"
#include "EvtHandlerAccessor.h"
#include "kxf/Utility/WithOptionalOwnership.h"

namespace kxf
{
	class KX_API EvtHandlerDelegate: public RTTI::ImplementInterface<IEvtHandler, IEvtHandler>
	{
		private:
			Utility::WithOptionalOwnership<IEvtHandler> m_EvtHandler;

		private:
			auto Access()
			{
				return EventSystem::EvtHandlerAccessor(*m_EvtHandler);
			}

		protected:
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<EventFlag> flags = {}) override
			{
				return Access().DoBind(eventID, std::move(executor), flags);
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor) override
			{
				return Access().DoUnbind(eventID, executor);
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot) override
			{
				return Access().DoUnbind(bindSlot);
			}

			bool OnDynamicBind(EventItem& eventItem) override
			{
				return Access().OnDynamicBind(eventItem);
			}
			bool OnDynamicUnbind(EventItem& eventItem) override
			{
				return Access().OnDynamicUnbind(eventItem);
			}

			void DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, UniversallyUniqueID uuid = {}, FlagSet<ProcessEventFlag> flags = {}) override
			{
				return Access().DoQueueEvent(std::move(event), eventID, std::move(uuid), flags);
			}
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, UniversallyUniqueID uuid = {}, FlagSet<ProcessEventFlag> flags = {}, IEvtHandler* onlyIn = nullptr) override
			{
				return Access().DoProcessEvent(event, eventID, std::move(uuid), flags, onlyIn);
			}

			bool TryBefore(IEvent& event) override
			{
				return Access().TryBefore(event);
			}
			bool TryAfter(IEvent& event) override
			{
				return Access().TryAfter(event);
			}

		public:
			EvtHandlerDelegate() noexcept = default;
			EvtHandlerDelegate(IEvtHandler& evtHandler) noexcept
			{
				m_EvtHandler.Assign(evtHandler);
			}
			EvtHandlerDelegate(std::unique_ptr<IEvtHandler> evtHandler) noexcept
			{
				m_EvtHandler.Assign(std::move(evtHandler));
			}
			EvtHandlerDelegate(EvtHandlerDelegate&&) noexcept = default;
			EvtHandlerDelegate(const EvtHandlerDelegate&) = delete;
			~EvtHandlerDelegate() = default;

		public:
			bool IsNull() const noexcept
			{
				return m_EvtHandler.IsNull();
			}
			IEvtHandler* Get() const noexcept
			{
				return m_EvtHandler.Get();
			}
			EvtHandlerDelegate ShallowClone() const noexcept
			{
				if (m_EvtHandler)
				{
					return *m_EvtHandler.Get();
				}
				return {};
			}

			// Event queuing and processing
			bool ProcessPendingEvents() override
			{
				return m_EvtHandler->ProcessPendingEvents();
			}
			size_t DiscardPendingEvents() override
			{
				return m_EvtHandler->DiscardPendingEvents();
			}

			// Event handlers chain
			IEvtHandler* GetPrevHandler() const override
			{
				return m_EvtHandler->GetPrevHandler();
			}
			IEvtHandler* GetNextHandler() const override
			{
				return m_EvtHandler->GetNextHandler();
			}
			void SetPrevHandler(IEvtHandler* evtHandler) override
			{
				m_EvtHandler->SetPrevHandler(evtHandler);
			}
			void SetNextHandler(IEvtHandler* evtHandler) override
			{
				m_EvtHandler->SetNextHandler(evtHandler);
			}

			void Unlink() override
			{
				m_EvtHandler->Unlink();
			}
			bool IsUnlinked() const override
			{
				return m_EvtHandler->IsUnlinked();
			}

			bool IsEventProcessingEnabled() const override
			{
				return m_EvtHandler->IsEventProcessingEnabled();
			}
			void EnableEventProcessing(bool enable = true) override
			{
				m_EvtHandler->EnableEventProcessing(enable);
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			EvtHandlerDelegate& operator=(EvtHandlerDelegate&&) noexcept = default;
			EvtHandlerDelegate& operator=(const EvtHandlerDelegate&) = delete;
	};
}
