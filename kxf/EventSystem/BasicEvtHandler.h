#pragma once
#include "EvtHandler.h"
#include "EvtHandlerAccessor.h"

namespace kxf
{
	template<class TDerived, class TBase>
	class BasicEvtHandler: public RTTI::PrivateStub<BasicEvtHandler<TDerived, TBase>, TBase>
	{
		private:
			TDerived& AsDerived() noexcept
			{
				return static_cast<TDerived&>(*this);
			}
			const TDerived& AsDerived() const noexcept
			{
				return static_cast<const TDerived&>(*this);
			}

		protected:
			EventSystem::EvtHandlerAccessor AccessThisEvtHandler() noexcept
			{
				return AsDerived().GetThisEvtHandler();
			}
			EventSystem::EvtHandlerAccessor AccessTopEvtHandler() noexcept
			{
				return AsDerived().GetTopEvtHandler();
			}

			// IEvtHandler
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<BindEventFlag> flags = {}) override
			{
				return AccessThisEvtHandler().DoBind(eventID, std::move(executor), flags);
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor) override
			{
				return AccessThisEvtHandler().DoUnbind(eventID, executor);
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot) override
			{
				return AccessThisEvtHandler().DoUnbind(bindSlot);
			}

			bool OnDynamicBind(EventSystem::EventItem& eventItem) override
			{
				return AccessThisEvtHandler().OnDynamicBind(eventItem);
			}
			bool OnDynamicUnbind(EventSystem::EventItem& eventItem) override
			{
				return AccessThisEvtHandler().OnDynamicUnbind(eventItem);
			}

			std::unique_ptr<IEvent> DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}) override
			{
				return AccessTopEvtHandler().DoQueueEvent(std::move(event), eventID, uuid, flags);
			}
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}, IEvtHandler* onlyIn = nullptr) override
			{
				return AccessTopEvtHandler().DoProcessEvent(event, eventID, uuid, flags, onlyIn);
			}

			bool TryBefore(IEvent& event) override
			{
				return AccessThisEvtHandler().TryBefore(event);
			}
			bool TryAfter(IEvent& event) override
			{
				return AccessThisEvtHandler().TryAfter(event);
			}

		public:
			// IEvtHandler
			bool ProcessPendingEvents() override
			{
				return AsDerived().GetThisEvtHandler().ProcessPendingEvents();
			}
			size_t DiscardPendingEvents() override
			{
				return AsDerived().GetThisEvtHandler().DiscardPendingEvents();
			}

			IEvtHandler* GetPrevHandler() const override
			{
				return AsDerived().GetThisEvtHandler().GetPrevHandler();
			}
			IEvtHandler* GetNextHandler() const override
			{
				return AsDerived().GetThisEvtHandler().GetNextHandler();
			}
			void SetPrevHandler(IEvtHandler* evtHandler) override
			{
				AsDerived().GetThisEvtHandler().SetPrevHandler(evtHandler);
			}
			void SetNextHandler(IEvtHandler* evtHandler) override
			{
				AsDerived().GetThisEvtHandler().SetNextHandler(evtHandler);
			}

			void Unlink() override
			{
				AsDerived().GetThisEvtHandler().Unlink();
			}
			bool IsUnlinked() const override
			{
				return AsDerived().GetThisEvtHandler().IsUnlinked();
			}

			bool IsEventProcessingEnabled() const override
			{
				return AsDerived().GetThisEvtHandler().IsEventProcessingEnabled();
			}
			void EnableEventProcessing(bool enable = true) override
			{
				AsDerived().GetThisEvtHandler().EnableEventProcessing(enable);
			}
	};
}
