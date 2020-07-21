#pragma once
#include "Common.h"
#include "Event.h"
#include "kxf/Utility/Common.h"

namespace kxf
{
	class KX_API WidgetEvent: public RTTI::ImplementInterface<WidgetEvent, BasicEvent, IWidgetEvent>
	{
		private:
			WidgetID m_WidgetID;
			size_t m_PropagationLevel = PropagationLevel::Max;

			String m_String;
			int64_t m_Int = 0;
			int64_t m_ExtraInt = 0;

		public:
			WidgetEvent() = default;
			WidgetEvent(const WidgetEvent&) = default;
			WidgetEvent(WidgetEvent&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetEvent>(std::move(*this));
			}
			FlagSet<EventCategory> GetEventCategory() const override
			{
				// This function is used to selectively process events in 'IEventLoop::YieldFor'
				// It returns 'EventCategory::UI' because 'WidgetEvent' is an UI event.
				return EventCategory::UI;
			}

			// IWidgetEvent
			WidgetID GetWidgetID() const override
			{
				return m_WidgetID;
			}
			void SetWidgetID(WidgetID id) override
			{
				m_WidgetID = id;
			}

			size_t StopPropagation() override
			{
				return Utility::ExchangeResetAndReturn(m_PropagationLevel, PropagationLevel::None);
			}
			void ResumePropagation(size_t level) override
			{
				m_PropagationLevel = level;
			}
			bool ShouldPropagate() const override
			{
				return m_PropagationLevel != PropagationLevel::None;
			}
			
			String GetString() const override
			{
				return m_String;
			}
			void SetString(const String& value) override
			{
				m_String = value;
			}
			void SetString(String&& value)
			{
				m_String = std::move(value);
			}

			int64_t GetInt() const override
			{
				return m_Int;
			}
			void SetInt(int64_t value) override
			{
				m_Int = value;
			}

			int64_t GetExtraInt() const override
			{
				return m_ExtraInt;
			}
			void SetExtraInt(int64_t value) override
			{
				m_ExtraInt = value;
			}

		public:
			WidgetEvent& operator=(const WidgetEvent&) = default;
			WidgetEvent& operator=(WidgetEvent&& other) noexcept
			{
				m_WidgetID = Utility::ExchangeResetAndReturn(other.m_WidgetID, wxID_NONE);
				m_PropagationLevel = Utility::ExchangeResetAndReturn(other.m_PropagationLevel, PropagationLevel::Max);
				m_String = std::move(other.m_String);

				return *this;
			}
	};
}
