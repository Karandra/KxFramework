#pragma once
#include "../Common.h"
#include "kxf/Compression/ArchiveEvent.h"
#include "kxf/System/ErrorCode.h"
#include "kxf/Utility/Common.h"
#include <7zip/CPP/7zip/IPassword.h>

#undef True
#undef False

namespace kxf::SevenZip::Private
{
	class PasswordHandler final
	{
		private:
			wxEvtHandler* m_EvtHandler = nullptr;
			EventID m_EventID = Event::EvtNull;

		private:
			ArchiveEvent CreateEvent(EventID id)
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

		public:
			PasswordHandler(EventID eventID, wxEvtHandler* evtHandler = nullptr) noexcept
				:m_EvtHandler(evtHandler), m_EventID(eventID)
			{
			}

		public:
			void SetEvtHandler(wxEvtHandler* evtHandler) noexcept
			{
				m_EvtHandler = evtHandler;
			}

			HResult OnPasswordRequest(BSTR* password, Int32* passwordIsDefined = nullptr);
	};
}
