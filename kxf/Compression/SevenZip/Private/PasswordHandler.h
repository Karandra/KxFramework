#pragma once
#include "../Common.h"
#include "kxf/Compression/ArchiveEvent.h"
#include "kxf/System/ErrorCode.h"
#include "kxf/Utility/Common.h"
#include <7zip/CPP/7zip/IPassword.h>

namespace kxf::SevenZip::Private
{
	class PasswordHandler final
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

		public:
			PasswordHandler(wxEvtHandler* evtHandler = nullptr) noexcept
				:m_EvtHandler(evtHandler)
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
