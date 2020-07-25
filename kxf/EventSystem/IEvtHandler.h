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

namespace kxf
{
	class IEventExecutor;
	class EvtHandlerDelegate;
}
namespace kxf::EventSystem
{
	class EvtHandlerAccessor;
}

namespace kxf
{
	class KX_API IEvtHandler:
		public RTTI::Interface<IEvtHandler>,
		public EventSystem::BindCRTP<IEvtHandler>,
		public EventSystem::CallAfterCRTP<IEvtHandler>,
		public EventSystem::QueueEventCRTP<IEvtHandler>,
		public EventSystem::ProcessEventCRTP<IEvtHandler>
	{
		KxDeclareIID(IEvtHandler, {0x96ae0970, 0x8cc5, 0x4288, {0xb1, 0x1b, 0x7a, 0xe6, 0x42, 0xf8, 0xdf, 0x8c}});

		friend class EventSystem::EvtHandlerAccessor;
		friend class EventSystem::BindCRTP<IEvtHandler>;
		friend class EventSystem::CallAfterCRTP<IEvtHandler>;
		friend class EventSystem::QueueEventCRTP<IEvtHandler>;
		friend class EventSystem::ProcessEventCRTP<IEvtHandler>;

		protected:
			using EventItem = EventSystem::EventItem;

		protected:
			virtual LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<EventFlag> flags = {}) = 0;
			virtual bool DoUnbind(const EventID& eventID, IEventExecutor& executor) = 0;
			virtual bool DoUnbind(const LocallyUniqueID& bindSlot) = 0;

			virtual bool OnDynamicBind(EventItem& eventItem) = 0;
			virtual bool OnDynamicUnbind(EventItem& eventItem) = 0;

			virtual void DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, UniversallyUniqueID uuid = {}) = 0;
			virtual bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, IEvtHandler* onlyIn = nullptr) = 0;
			virtual bool DoProcessEventSafely(IEvent& event, const EventID& eventID = {}) = 0;
			virtual bool DoProcessEventLocally(IEvent& event, const EventID& eventID = {}) = 0;

			virtual bool TryBefore(IEvent& event) = 0;
			virtual bool TryAfter(IEvent& event) = 0;

		public:
			IEvtHandler() = default;
			virtual ~IEvtHandler() = default;

		public:
			// Event queuing and processing
			virtual bool ProcessPendingEvents() = 0;
			virtual size_t DiscardPendingEvents() = 0;

			// Event handlers chain
			virtual IEvtHandler* GetPrevHandler() const = 0;
			virtual IEvtHandler* GetNextHandler() const = 0;
			virtual void SetPrevHandler(IEvtHandler* evtHandler) = 0;
			virtual void SetNextHandler(IEvtHandler* evtHandler) = 0;

			virtual void Unlink() = 0;
			virtual bool IsUnlinked() const = 0;

			virtual bool IsEventProcessingEnabled() const = 0;
			virtual void EnableEventProcessing(bool enable = true) = 0;
	};
}
