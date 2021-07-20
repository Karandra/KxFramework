#pragma once
#include "Common.h"
#include "kxf/Utility/Common.h"

namespace kxf
{
	class KX_API WidgetEvent: public RTTI::DynamicImplementation<WidgetEvent, BasicEvent, IWidgetEvent>
	{
		KxRTTI_DeclareIID(WidgetEvent, {0xa7e0c7a2, 0xe2cf, 0x4b11, {0xa4, 0x1b, 0x9, 0x77, 0x47, 0x6a, 0xa7, 0x70}});

		private:
			std::shared_ptr<IWidget> m_Widget;
			WidgetID m_WidgetID;
			size_t m_PropagationLevel = PropagationLevel::Max;

			String m_String;
			int64_t m_Int = 0;
			int64_t m_ExtraInt = 0;

		public:
			WidgetEvent() noexcept = default;
			WidgetEvent(IWidget& widget) noexcept;
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
				return EventCategory::UI;
			}

			// IWidgetEvent
			std::shared_ptr<IWidget> GetWidget() const override
			{
				return m_Widget;
			}
			void SetWidget(std::shared_ptr<IWidget> widget) override
			{
				m_Widget = std::move(widget);
			}

			WidgetID GetWidgetID() const override;
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
				static_cast<BasicEvent&>(*this) = std::move(other);

				m_Widget = std::move(other.m_Widget);
				m_WidgetID = Utility::ExchangeResetAndReturn(other.m_WidgetID, StdID::None);
				m_PropagationLevel = Utility::ExchangeResetAndReturn(other.m_PropagationLevel, PropagationLevel::Max);

				m_String = std::move(other.m_String);
				m_Int = Utility::ExchangeResetAndReturn(other.m_Int, 0);
				m_ExtraInt = Utility::ExchangeResetAndReturn(other.m_ExtraInt, 0);

				return *this;
			}
	};
}
