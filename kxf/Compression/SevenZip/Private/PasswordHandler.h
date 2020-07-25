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
			IEvtHandler* m_EvtHandler = nullptr;
			EventID m_EventID = IEvent::EvtNull;

		private:
			ArchiveEvent CreateEvent()
			{
				ArchiveEvent event;
				event.Allow();
				
				return event;
			}
			bool SendEvent(ArchiveEvent& event, const EventID& eventID)
			{
				if (m_EvtHandler->ProcessEvent(event, eventID) && !event.IsSkipped())
				{
					return event.IsAllowed();
				}
				return true;
			}

		public:
			PasswordHandler(EventID eventID, IEvtHandler* evtHandler = nullptr) noexcept
				:m_EvtHandler(evtHandler), m_EventID(eventID)
			{
			}

		public:
			void SetEvtHandler(IEvtHandler* evtHandler) noexcept
			{
				m_EvtHandler = evtHandler;
			}

			HResult OnPasswordRequest(BSTR* password, Int32* passwordIsDefined = nullptr);
	};
}
