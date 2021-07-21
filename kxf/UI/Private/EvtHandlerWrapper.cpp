#include "KxfPCH.h"
#include "EvtHandlerWrapper.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "../IGraphicsRendererAwareWidget.h"

#include "../Events/WidgetEvent.h"
#include "../Events/WidgetKeyEvent.h"
#include "../Events/WidgetSizeEvent.h"
#include "../Events/WidgetDrawEvent.h"
#include "../Events/WidgetFocusEvent.h"
#include "../Events/WidgetLifetimeEvent.h"
#include "../Events/WidgetContextMenuEvent.h"

namespace kxf::WXUI::Private
{
	class WxWidgetPaintEvent final: public WidgetDrawEvent
	{
		public:
			WxWidgetPaintEvent(IWidget& widget) noexcept
				:WidgetDrawEvent(widget, nullptr)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WxWidgetPaintEvent>(std::move(*this));
			}

			// WidgetDrawEvent
			std::shared_ptr<IGraphicsContext> GetGraphicsContext() override
			{
				if (!m_GraphicsContext)
				{
					auto widget = GetWidget();
					if (auto rendererAware = widget->QueryInterface<IGraphicsRendererAwareWidget>())
					{
						m_GraphicsContext = rendererAware->GetActiveGraphicsRenderer()->CreateWidgetPaintContext(*widget);
					}
					if (!m_GraphicsContext)
					{
						m_GraphicsContext = Drawing::GetDefaultRenderer()->CreateWidgetPaintContext(*widget);
					}
				}
				return m_GraphicsContext;
			}
	};

	class WxWidgetEraseEvent final: public WidgetDrawEvent
	{
		private:
			wxDC& m_DC;

		public:
			WxWidgetEraseEvent(IWidget& widget, wxDC& dc) noexcept
				:WidgetDrawEvent(widget, nullptr), m_DC(dc)
			{
			}

		public:
			// IEvent
			std::unique_ptr<IEvent> Move() noexcept override
			{
				return std::make_unique<WxWidgetEraseEvent>(std::move(*this));
			}

			// WidgetDrawEvent
			std::shared_ptr<IGraphicsContext> GetGraphicsContext() override
			{
				if (!m_GraphicsContext)
				{
					auto widget = GetWidget();
					if (auto rendererAware = widget->QueryInterface<IGraphicsRendererAwareWidget>())
					{
						m_GraphicsContext = rendererAware->GetActiveGraphicsRenderer()->CreateLegacyContext(m_DC, widget->GetSize(WidgetSizeFlag::Client));
					}
					if (!m_GraphicsContext)
					{
						m_GraphicsContext = Drawing::GetDefaultRenderer()->CreateLegacyContext(m_DC, widget->GetSize(WidgetSizeFlag::Client));
					}
				}
				return m_GraphicsContext;
			}
	};
}

namespace kxf::WXUI::Private
{
	bool EvtHandlerWrapperBase::TryBefore(wxEvent& anyEvent)
	{
		const auto eventType = anyEvent.GetEventType();
		if (eventType == wxEVT_DESTROY)
		{
			m_Widget.ProcessEvent(WidgetLifetimeEvent::EvtDestroyed, m_Widget);
		}
		else if (eventType == wxEVT_SIZE)
		{
			auto& event = static_cast<wxSizeEvent&>(anyEvent);

			auto rect = Rect(event.GetRect());
			rect.SetSize(Size(event.GetSize()));
			m_Widget.ProcessEvent(WidgetSizeEvent::EvtSize, m_Widget, rect);
		}
		else if (eventType == wxEVT_PAINT)
		{
			WxWidgetPaintEvent event(m_Widget);
			return m_Widget.ProcessEvent(event, WidgetDrawEvent::EvtDrawContent);
		}
		else if (eventType == wxEVT_ERASE_BACKGROUND)
		{
			auto& event = static_cast<wxEraseEvent&>(anyEvent);
			if (wxDC* dc = event.GetDC())
			{
				WxWidgetEraseEvent event(m_Widget, *dc);
				return m_Widget.ProcessEvent(event, WidgetDrawEvent::EvtDrawBackground);
			}
		}
		else if (eventType == wxEVT_CONTEXT_MENU)
		{
			auto& event = static_cast<wxContextMenuEvent&>(anyEvent);
			return m_Widget.ProcessEvent(WidgetContextMenuEvent::EvtShow, m_Widget, Point(event.GetPosition()));
		}
		else if (eventType == wxEVT_SET_FOCUS)
		{
			return m_Widget.ProcessEvent(WidgetFocusEvent::EvtFocusReceived, m_Widget);
		}
		else if (eventType == wxEVT_KILL_FOCUS)
		{
			return m_Widget.ProcessEvent(WidgetFocusEvent::EvtFocusLost, m_Widget);
		}
		else if (eventType == wxEVT_KEY_UP || eventType == wxEVT_KEY_DOWN || eventType == wxEVT_CHAR || eventType == wxEVT_CHAR_HOOK)
		{
			auto& event = static_cast<wxKeyEvent&>(anyEvent);
			if (eventType == wxEVT_KEY_UP)
			{
				return m_Widget.ProcessEvent(WidgetKeyEvent::EvtKeyUp, m_Widget, event);
			}
			else if (eventType == wxEVT_KEY_DOWN)
			{
				return m_Widget.ProcessEvent(WidgetKeyEvent::EvtKeyDown, m_Widget, event);
			}
			else if (eventType == wxEVT_CHAR)
			{
				return m_Widget.ProcessEvent(WidgetKeyEvent::EvtChar, m_Widget, event);
			}
			else if (eventType == wxEVT_CHAR_HOOK)
			{
				WidgetKeyEvent keyEvent(m_Widget, event);
				const bool result = m_Widget.ProcessEvent(keyEvent, WidgetKeyEvent::EvtCharHook);

				if (keyEvent.IsNextEventAllowed())
				{
					event.DoAllowNextEvent();
				}
				return result;
			}
		}
		return false;
	}
	bool EvtHandlerWrapperBase::TryAfter(wxEvent& anyEvent)
	{
		const auto eventType = anyEvent.GetEventType();
		if (eventType == wxEVT_CREATE)
		{
			m_Widget.ProcessEvent(WidgetLifetimeEvent::EvtCreated, m_Widget);
		}
		return false;
	}

	bool EvtHandlerWrapperBase::OnDynamicBind(wxDynamicEventTableEntry& eventItem)
	{
		return true;
	}
}
