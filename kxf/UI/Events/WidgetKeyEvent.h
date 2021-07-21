#pragma once
#include "WidgetEvent.h"
#include "kxf/General/UniChar.h"
class wxKeyEvent;

namespace kxf
{
	class KX_API WidgetKeyEvent: public WidgetEvent
	{
		public:
			KxEVENT_MEMBER(WidgetKeyEvent, KeyUp);
			KxEVENT_MEMBER(WidgetKeyEvent, KeyDown);

			KxEVENT_MEMBER(WidgetKeyEvent, Char);
			KxEVENT_MEMBER(WidgetKeyEvent, CharHook);

		private:
			Point m_Position;
			KeyCode m_KeyCode = KeyCode::None;
			UniChar m_UnicodeKey;
			uint32_t m_NativeKeyCode = 0;
			FlagSet<uint32_t> m_NativeKeyFlags;
			bool m_IsNextEventAllowed = false;
			bool m_IsAutoRepeat = false;

		public:
			WidgetKeyEvent() noexcept = default;
			WidgetKeyEvent(IWidget& widget, KeyCode keyCode) noexcept;
			WidgetKeyEvent(IWidget& widget, uint32_t nativeKeyCode, FlagSet<uint32_t> nativeKeyFlags) noexcept;
			WidgetKeyEvent(IWidget& widget, const wxKeyEvent& keyEventWX) noexcept;

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WidgetKeyEvent>(std::move(*this));
			}
			FlagSet<EventCategory> GetEventCategory() const override
			{
				return EventCategory::UserInput;
			}

			// WidgetKeyEvent
			KeyCode GetKeyCode() const noexcept
			{
				return m_KeyCode;
			}
			UniChar GetUnicodeKey() const noexcept
			{
				return m_UnicodeKey;
			}
			Point GetPosition() const noexcept
			{
				return m_Position;
			}
			bool IsAutoRepeat() const noexcept
			{
				return m_IsAutoRepeat;
			}
			bool IsKeyInCategory(FlagSet<KeyCategory> category) const noexcept;

			bool IsNextEventAllowed() const noexcept
			{
				return m_IsNextEventAllowed;
			}
			void AllowNextEvent(bool allow = true) noexcept
			{
				m_IsNextEventAllowed = allow;
			}

			uint32_t GetNativeKeyCode() const noexcept
			{
				return m_NativeKeyCode;
			}
			FlagSet<uint32_t> GetNativeKeyFlags() const noexcept
			{
				return m_NativeKeyFlags;
			}
	};
}
