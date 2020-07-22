#pragma once
#include "kxf/EventSystem/IEvent.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/Utility/WithOptionalOwnership.h"
#include <wx/event.h>

namespace kxf::wxWidgets
{
	class KX_API EventWrapper: public RTTI::ImplementInterface<EventWrapper, IEvent>
	{
		private:
			Utility::WithOptionalOwnership<wxEvent> m_Event;
			UniversallyUniqueID m_UniqueID;

		private:
			bool WasProcessed() const override
			{
				return m_Event->WasProcessed();
			}
			bool WillBeProcessedAgain() const override
			{
				return m_Event->WillBeProcessedAgain();
			}

			void OnStartProcess(const EventID& eventID, const UniversallyUniqueID& uuid) override
			{
				if (eventID && m_Event->GetEventType() == wxEVT_NULL)
				{
					m_Event->SetEventType(eventID.AsInt());
				}
				if (!m_UniqueID)
				{
					m_UniqueID = std::move(uuid);
				}
				if (m_Event->GetTimestamp() < 0)
				{
					m_Event->SetTimestamp(TimeSpan::Now(SteadyClock()).GetMilliseconds());
				}
			}

		public:
			EventWrapper(wxEvent& evtHandler)
				:m_Event(evtHandler)
			{
			}
			EventWrapper(std::unique_ptr<wxEvent> evtHandler)
				:m_Event(std::move(evtHandler))
			{
			}
			EventWrapper(EventWrapper&& other) noexcept = default;
			EventWrapper(const EventWrapper&) = delete;

		public:
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<EventWrapper>(std::move(*this));
			}

			EventID GetEventID() const override
			{
				return m_Event->GetEventType();
			}
			TimeSpan GetTimestamp() const override
			{
				return TimeSpan::Milliseconds(m_Event->GetTimestamp());
			}
			UniversallyUniqueID GetUniqueID() const override
			{
				return m_UniqueID;
			}
			FlagSet<EventCategory> GetEventCategory() const override
			{
				return static_cast<EventCategory>(m_Event->GetEventCategory());
			}

			EvtHandler* GetEventSource() const override
			{
				return dynamic_cast<EvtHandler*>(m_Event->GetEventObject());
			}
			void SetEventSource(EvtHandler* evtHandler) override
			{
				m_Event->SetEventObject(dynamic_cast<wxObject*>(evtHandler));
			}
			
			bool IsSkipped() const override
			{
				return m_Event->GetSkipped();
			}
			void Skip(bool skip = true) override
			{
				m_Event->Skip(skip);
			}

			bool IsAllowed() const override
			{
				if (m_Event->IsKindOf(wxCLASSINFO(wxNotifyEvent)))
				{
					return static_cast<const wxNotifyEvent&>(*m_Event).IsAllowed();
				}
				return true;
			}
			void Allow(bool allow = true) override
			{
				if (m_Event->IsKindOf(wxCLASSINFO(wxNotifyEvent)))
				{
					wxNotifyEvent& notifyEvent = static_cast<wxNotifyEvent&>(*m_Event);
					if (allow)
					{
						notifyEvent.Allow();
					}
					else
					{
						notifyEvent.Veto();
					}
				}
			}

		public:
			EventWrapper& operator=(EventWrapper&& other) noexcept = default;
			EventWrapper& operator=(const EventWrapper&) = delete;
	};
}
