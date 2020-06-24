#pragma once
#include "../Common.h"
#include "kxf/Compression/ArchiveEvent.h"

namespace kxf::SevenZip::Private
{
	class WithEvtHandler
	{
		protected:
			wxEvtHandler* m_EvtHandler = nullptr;

		protected:
			ArchiveEvent CreateEvent(EventID id = ArchiveEvent::EvtProcess)
			{
				ArchiveEvent event(id);
				event.SetEventObject(m_EvtHandler);
				event.Allow();
				
				return event;
			}
			bool SendEvent(ArchiveEvent& event)
			{
				if (m_EvtHandler->ProcessEvent(event) && !event.GetSkipped())
				{
					return event.IsAllowed();
				}
				return true;
			}

		protected:
			WithEvtHandler(wxEvtHandler* evtHandler = nullptr) noexcept
				:m_EvtHandler(evtHandler)
			{
			}
			~WithEvtHandler() = default;

		public:
			wxEvtHandler* GetEvtHandler() const noexcept
			{
				return m_EvtHandler;
			}
			virtual void SetEvtHandler(wxEvtHandler* evtHandler) noexcept
			{
				m_EvtHandler = evtHandler;
			}
	};
}
