#pragma once
#include "EventWrapper.h"
#include "ClientObject.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/Core/OptionalPtr.h"
#include <wx/event.h>

namespace kxf::wxWidgets
{
	inline bool ForwardBind(IEvtHandler& evtHandler, wxEvtHandler& evtHandlerWx, EventSystem::EventItem& eventItem)
	{
		if (EventID id = eventItem.GetEventID(); id.IsWxWidgetsID())
		{
			evtHandlerWx.Bind(wxEventTypeTag<wxEvent>(id.AsInt()), [evtHandler = &evtHandler, executor = eventItem.GetExecutor()](wxEvent& event)
			{
				EventWrapper wrapper(event);
				executor->Execute(*evtHandler, wrapper);
			}, wxID_ANY, wxID_ANY, new ClientObject(eventItem.GetBindSlot()));
			return true;
		}
		return false;
	}
	inline bool InjectBeforeEvtHandler(IEvtHandler& evtHandler, wxEvent& event)
	{
		wxWidgets::EventWrapper wrapper(event);
		return evtHandler.ProcessEvent(wrapper, event.GetEventType(), ProcessEventFlag::Locally);
	}
}

namespace kxf::wxWidgets
{
	class EvtHandlerWrapper: public EvtHandler
	{
		private:
			optional_ptr<wxEvtHandler> m_EvtHandler;

		protected:
			bool OnDynamicBind(EventItem& eventItem) override
			{
				if (EvtHandler::OnDynamicBind(eventItem))
				{
					ForwardBind(*this, *m_EvtHandler, eventItem);
					return true;
				}
				return false;
			}
			bool OnDynamicUnbind(EventItem& eventItem) override
			{
				// We can't really reliably unbind event in 'wxEvtHandler' given our internals
				return EvtHandler::OnDynamicUnbind(eventItem);
			}

		public:
			EvtHandlerWrapper(wxEvtHandler& evtHandler)
				:m_EvtHandler(evtHandler)
			{
			}
			EvtHandlerWrapper(std::unique_ptr<wxEvtHandler> evtHandler)
				:m_EvtHandler(std::move(evtHandler))
			{
			}
			EvtHandlerWrapper(EvtHandlerWrapper&& other) noexcept = default;
			EvtHandlerWrapper(const EvtHandlerWrapper&) = delete;

		public:
			EvtHandlerWrapper& operator=(EvtHandlerWrapper&& other) noexcept = default;
			EvtHandlerWrapper& operator=(const EvtHandlerWrapper&) = delete;
	};
}
