#pragma once
#include "Common.h"
#include "IEvtHandler.h"

namespace kxf::EventSystem
{
	class EvtHandlerAccessor final
	{
		private:
			IEvtHandler& m_EvtHandler;

		public:
			EvtHandlerAccessor(IEvtHandler& evtHandler)
				:m_EvtHandler(evtHandler)
			{
			}

		public:
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<EventFlag> flags = {})
			{
				return m_EvtHandler.DoBind(eventID, std::move(executor), flags);
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor)
			{
				return m_EvtHandler.DoUnbind(eventID, executor);
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot)
			{
				return m_EvtHandler.DoUnbind(bindSlot);
			}

			bool OnDynamicBind(EventItem& eventItem)
			{
				return m_EvtHandler.OnDynamicBind(eventItem);
			}
			bool OnDynamicUnbind(EventItem& eventItem)
			{
				return m_EvtHandler.OnDynamicUnbind(eventItem);
			}

			void DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, UniversallyUniqueID uuid = {}, FlagSet<ProcessEventFlag> flags = {})
			{
				m_EvtHandler.DoQueueEvent(std::move(event), eventID, std::move(uuid), flags);
			}
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, UniversallyUniqueID uuid = {}, FlagSet<ProcessEventFlag> flags = {}, IEvtHandler* onlyIn = nullptr)
			{
				return m_EvtHandler.DoProcessEvent(event, eventID, std::move(uuid), flags);
			}

			bool TryBefore(IEvent& event)
			{
				return m_EvtHandler.TryBefore(event);
			}
			bool TryAfter(IEvent& event)
			{
				return m_EvtHandler.TryAfter(event);
			}
	};
}
