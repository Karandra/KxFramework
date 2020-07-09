#pragma once
#include "Common.h"
#include "Event.h"

namespace kxf
{
	enum class IdleEventMode
	{
		ProcessAll = wxIDLE_PROCESS_ALL,
		ProcessSpecific = wxIDLE_PROCESS_SPECIFIED
	};
}

namespace kxf
{
	class KX_API IdleEvent: public Event
	{
		public:
			static inline const EventTag<IdleEvent> EvtIdle = wxEVT_IDLE;

		public:
			static IdleEventMode GetMode() noexcept
			{
				return static_cast<IdleEventMode>(wxIdleEvent::GetMode());
			}
			static void SetMode(IdleEventMode mode) noexcept
			{
				wxIdleEvent::SetMode(static_cast<wxIdleMode>(mode));
			}

		private:
			bool m_RequestedMore = false;

		public:
			IdleEvent() noexcept = default;

		public:
			std::unique_ptr<Event> Move() noexcept override
			{
				return std::make_unique<IdleEvent>(std::move(*this));
			}

			bool IsMoreRequested() const noexcept
			{
				return m_RequestedMore;
			}
			void RequestMore(bool needMore = true) noexcept
			{
				m_RequestedMore = needMore;
			}
	};
}
