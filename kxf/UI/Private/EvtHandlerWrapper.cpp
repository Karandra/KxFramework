#include "KxfPCH.h"
#include "EvtHandlerWrapper.h"
#include "kxf/Network/URI.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "../IGraphicsRendererAwareWidget.h"

#include "../Events/WidgetEvent.h"
#include "../Events/WidgetTextEvent.h"
#include "../Events/WidgetKeyEvent.h"
#include "../Events/WidgetSizeEvent.h"
#include "../Events/WidgetDrawEvent.h"
#include "../Events/WidgetMouseEvent.h"
#include "../Events/WidgetFocusEvent.h"
#include "../Events/WidgetLifetimeEvent.h"
#include "../Events/WidgetContextMenuEvent.h"

namespace kxf::WXUI::Private
{
	EventID MapMouseEvent(const wxEventType& eventType) noexcept
	{
		if (eventType == wxEVT_LEFT_UP)
		{
			return WidgetMouseEvent::EvtLeftUp;
		}
		else if (eventType == wxEVT_LEFT_DOWN)
		{
			return WidgetMouseEvent::EvtLeftDown;
		}
		else if (eventType == wxEVT_LEFT_DCLICK)
		{
			return WidgetMouseEvent::EvtLeftDoubleClick;
		}

		else if (eventType == wxEVT_RIGHT_UP)
		{
			return WidgetMouseEvent::EvtRightUp;
		}
		else if (eventType == wxEVT_RIGHT_DOWN)
		{
			return WidgetMouseEvent::EvtRightDown;
		}
		else if (eventType == wxEVT_RIGHT_DCLICK)
		{
			return WidgetMouseEvent::EvtRightDoubleClick;
		}

		else if (eventType == wxEVT_MIDDLE_UP)
		{
			return WidgetMouseEvent::EvtMiddleUp;
		}
		else if (eventType == wxEVT_MIDDLE_DOWN)
		{
			return WidgetMouseEvent::EvtMiddleDown;
		}
		else if (eventType == wxEVT_MIDDLE_DCLICK)
		{
			return WidgetMouseEvent::EvtMiddleDoubleClick;
		}

		else if (eventType == wxEVT_AUX1_UP)
		{
			return WidgetMouseEvent::EvtAux1Up;
		}
		else if (eventType == wxEVT_AUX1_DOWN)
		{
			return WidgetMouseEvent::EvtAux1Down;
		}
		else if (eventType == wxEVT_AUX1_DCLICK)
		{
			return WidgetMouseEvent::EvtAux1DoubleClick;
		}

		else if (eventType == wxEVT_AUX2_UP)
		{
			return WidgetMouseEvent::EvtAux2Up;
		}
		else if (eventType == wxEVT_AUX2_DOWN)
		{
			return WidgetMouseEvent::EvtAux2Down;
		}
		else if (eventType == wxEVT_AUX2_DCLICK)
		{
			return WidgetMouseEvent::EvtAux2DoubleClick;
		}

		else if (eventType == wxEVT_MOTION)
		{
			return WidgetMouseEvent::EvtMove;
		}
		else if (eventType == wxEVT_ENTER_WINDOW)
		{
			return WidgetMouseEvent::EvtEnter;
		}
		else if (eventType == wxEVT_LEAVE_WINDOW)
		{
			return WidgetMouseEvent::EvtLeave;
		}
		else if (eventType == wxEVT_MOUSEWHEEL)
		{
			return WidgetMouseEvent::EvtWheel;
		}
		else if (eventType == wxEVT_MAGNIFY)
		{
			return WidgetMouseEvent::EvtMagnification;
		}
		return {};
	}
	EventID MapKeyboardEvent(const wxEventType& eventType) noexcept
	{
		if (eventType == wxEVT_KEY_UP)
		{
			return WidgetKeyEvent::EvtKeyUp;
		}
		else if (eventType == wxEVT_KEY_DOWN)
		{
			return WidgetKeyEvent::EvtKeyDown;
		}
		else if (eventType == wxEVT_CHAR)
		{
			return WidgetKeyEvent::EvtChar;
		}
		else if (eventType == wxEVT_CHAR_HOOK)
		{
			return WidgetKeyEvent::EvtCharHook;
		}
		return {};
	}

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
		else if (auto eventID = MapKeyboardEvent(eventType))
		{
			auto& event = static_cast<wxKeyEvent&>(anyEvent);

			WidgetKeyEvent keyEvent(m_Widget, event);
			const bool result = m_Widget.ProcessEvent(keyEvent, eventID);
			if (eventID == WidgetKeyEvent::EvtCharHook && keyEvent.IsNextEventAllowed())
			{
				event.DoAllowNextEvent();
			}
			return result;
		}
		else if (auto eventID = MapMouseEvent(eventType))
		{
			auto& event = static_cast<wxMouseEvent&>(anyEvent);

			WidgetMouseEvent mouseEvent(m_Widget, event);
			return m_Widget.ProcessEvent(mouseEvent, eventID);
		}
		else if (eventType == wxEVT_TEXT || eventType == wxEVT_TEXT_ENTER || eventType == wxEVT_TEXT_MAXLEN ||
				 eventType == wxEVT_TEXT_URL || eventType == wxEVT_TEXT_CUT || eventType == wxEVT_TEXT_COPY ||
				 eventType == wxEVT_TEXT_PASTE)
		{
			auto& event = static_cast<wxTextUrlEvent&>(anyEvent);
			if (eventType == wxEVT_TEXT)
			{
				return m_Widget.ProcessEvent(WidgetTextEvent::EvtChanged, m_Widget, event.GetString());
			}
			else if (eventType == wxEVT_TEXT_CUT)
			{
				return m_Widget.ProcessEvent(WidgetTextEvent::EvtCut, m_Widget, event.GetString());
			}
			else if (eventType == wxEVT_TEXT_COPY)
			{
				return m_Widget.ProcessEvent(WidgetTextEvent::EvtCopy, m_Widget, event.GetString());
			}
			else if (eventType == wxEVT_TEXT_PASTE)
			{
				return m_Widget.ProcessEvent(WidgetTextEvent::EvtPaste, m_Widget, event.GetString());
			}
			else if (eventType == wxEVT_TEXT_ENTER)
			{
				return m_Widget.ProcessEvent(WidgetTextEvent::EvtCommit, m_Widget);
			}
			else if (eventType == wxEVT_TEXT_MAXLEN)
			{
				// No way to know the actual length limit here
				return m_Widget.ProcessEvent(WidgetTextEvent::EvtLengthLimit, m_Widget);
			}
			else if (eventType == wxEVT_TEXT_URL)
			{
				WidgetMouseEvent mouseEvent(m_Widget, event.GetMouseEvent());
				return m_Widget.ProcessEvent(WidgetTextEvent::EvtURI, m_Widget, event.GetString(), std::move(mouseEvent));
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
