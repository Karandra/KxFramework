#pragma once
#include "../Common.h"
#include "kxf/Compression/ArchiveEvent.h"

namespace kxf::SevenZip::Private
{
	class WithEvtHandler
	{
		protected:
			EvtHandlerDelegate m_EvtHandler;

		public:
			WithEvtHandler(IEvtHandler* evtHandler = nullptr) noexcept
			{
				WithEvtHandler::SetEvtHandler(evtHandler);
			}
			WithEvtHandler(WithEvtHandler&& other) noexcept
				:m_EvtHandler(std::move(other.m_EvtHandler))
			{
				other.SetEvtHandler(nullptr);
			}
			WithEvtHandler(const WithEvtHandler&) noexcept = default;
			~WithEvtHandler() = default;

		public:
			IEvtHandler* GetEvtHandler() const noexcept
			{
				return m_EvtHandler.Get();
			}
			virtual void SetEvtHandler(IEvtHandler* evtHandler) noexcept
			{
				if (evtHandler)
				{
					m_EvtHandler = *evtHandler;
				}
				else
				{
					m_EvtHandler = {};
				}
			}

			ArchiveEvent CreateEvent()
			{
				ArchiveEvent event;
				event.Allow();
				
				return event;
			}
			bool SendEvent(ArchiveEvent& event, const EventID& id)
			{
				if (m_EvtHandler.ProcessEvent(event) && !event.IsSkipped())
				{
					return event.IsAllowed();
				}
				return true;
			}

		public:
			WithEvtHandler& operator=(WithEvtHandler&& other) noexcept
			{
				m_EvtHandler = std::move(other.m_EvtHandler);
				other.SetEvtHandler(nullptr);

				return *this;
			}
			WithEvtHandler& operator=(const WithEvtHandler&) noexcept = default;

			explicit operator bool() const noexcept
			{
				return !m_EvtHandler.IsNull();
			}
			bool operator!() const noexcept
			{
				return m_EvtHandler.IsNull();
			}
	};
}
