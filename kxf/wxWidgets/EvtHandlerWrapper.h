#pragma once
#include "ClientObject.h"
#include "EventWrapper.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/Utility/WithOptionalOwnership.h"
#include <wx/event.h>

namespace kxf::wxWidgets
{
	class EvtHandlerWrapper: public EvtHandler
	{
		public:
			static void ForwardBind(EvtHandler& evtHandler, wxEvtHandler& evtHandlerWx, EventItem& eventItem)
			{
				if (EventID id = eventItem.GetEventID(); id.IsWxWidgetsID())
				{
					return evtHandlerWx.Bind(wxEventTypeTag<wxEvent>(id.AsInt()), [evtHandler = &evtHandler, executor = eventItem.GetExecutor()](wxEvent& event)
					{
						EventWrapper wrapper(event);
						executor->Execute(*evtHandler, wrapper);
					}, wxID_ANY, wxID_ANY, new ClientObject(eventItem.GetBindSlot()));
				}
			}

		private:
			Utility::WithOptionalOwnership<wxEvtHandler> m_EvtHandler;

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
