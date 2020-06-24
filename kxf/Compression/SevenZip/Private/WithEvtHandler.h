#pragma once
#include "../Common.h"
#include "kxf/Compression/ArchiveEvent.h"

namespace kxf::SevenZip::Private
{
	class WithEvtHandler
	{
		protected:
			wxEvtHandler* m_EvtHandler = nullptr;

		public:
			WithEvtHandler(wxEvtHandler* evtHandler = nullptr) noexcept
				:m_EvtHandler(evtHandler)
			{
			}
			WithEvtHandler(WithEvtHandler&& other) noexcept
				:m_EvtHandler(other.m_EvtHandler)
			{
				other.SetEvtHandler(nullptr);
			}
			WithEvtHandler(const WithEvtHandler&) noexcept = default;
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
			WithEvtHandler& operator=(WithEvtHandler&& other) noexcept
			{
				m_EvtHandler = other.m_EvtHandler;
				other.SetEvtHandler(nullptr);

				return *this;
			}
			WithEvtHandler& operator=(const WithEvtHandler&) noexcept = default;

			explicit operator bool() const noexcept
			{
				return m_EvtHandler != nullptr;
			}
			bool operator!() const noexcept
			{
				return m_EvtHandler == nullptr;
			}
	};
}
