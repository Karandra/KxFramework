#pragma once
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/RTTI/QueryInterface.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/WithOptionalOwnership.h"
#include <wx/event.h>

namespace kxf::wxWidgets
{
	class wxEvtHandlerWrapper: public EvtHandler
	{
		private:
			Utility::WithOptionalOwnership<wxEvtHandler> m_EvtHandler;

		private:

		public:
			wxEvtHandlerWrapper(wxEvtHandler& evtHandler)
				:m_EvtHandler(evtHandler)
			{
			}
			wxEvtHandlerWrapper(std::unique_ptr<wxEvtHandler> evtHandler)
				:m_EvtHandler(std::move(evtHandler))
			{
			}
			wxEvtHandlerWrapper(wxEvtHandlerWrapper&& other) noexcept = default;
			wxEvtHandlerWrapper(const wxEvtHandlerWrapper&) = delete;

		public:
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<EventFlag> flags = {}) override
			{
				return {};
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor) override
			{
				return false;
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot) override
			{
				return false;
			}

			void DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, UniversallyUniqueID uuid = {}) override
			{
			}
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, EvtHandler* onlyIn = nullptr) override
			{
				return false;
			}

			bool TryBefore(IEvent& event) override
			{
				return false;
			}
			bool TryAfter(IEvent& event) override
			{
				return false;
			}

		public:
			wxEvtHandlerWrapper& operator=(wxEvtHandlerWrapper&& other) noexcept = default;
			wxEvtHandlerWrapper& operator=(const wxEvtHandlerWrapper&) = delete;
	};
}
