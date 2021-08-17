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
#include "../Events/WidgetScrollEvent.h"
#include "../Events/WidgetLifetimeEvent.h"
#include "../Events/WidgetContextMenuEvent.h"

#include "../ITextWidget.h"

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

	EventID MapScrollEventEvent(const wxEventType& eventType) noexcept
	{
		if (eventType == wxEVT_SCROLL_TOP)
		{
			return WidgetScrollEvent::EvtTop;
		}
		else if (eventType == wxEVT_SCROLL_BOTTOM)
		{
			return WidgetScrollEvent::EvtBottom;
		}
		else if (eventType == wxEVT_SCROLL_LINEUP)
		{
			return WidgetScrollEvent::EvtLineUp;
		}
		else if (eventType == wxEVT_SCROLL_LINEDOWN)
		{
			return WidgetScrollEvent::EvtLineDown;
		}
		else if (eventType == wxEVT_SCROLL_PAGEUP)
		{
			return WidgetScrollEvent::EvtPageUp;
		}
		else if (eventType == wxEVT_SCROLL_PAGEDOWN)
		{
			return WidgetScrollEvent::EvtPageDown;
		}
		else if (eventType == wxEVT_SCROLL_THUMBTRACK || eventType == wxEVT_SCROLL_CHANGED)
		{
			return WidgetScrollEvent::EvtThumbTrack;
		}
		else if (eventType == wxEVT_SCROLL_THUMBRELEASE)
		{
			return WidgetScrollEvent::EvtThumbRelease;
		}
		return {};
	}
	EventID MapScrollWinEventEvent(const wxEventType& eventType) noexcept
	{
		if (eventType == wxEVT_SCROLLWIN_TOP)
		{
			return WidgetScrollEvent::EvtTop;
		}
		else if (eventType == wxEVT_SCROLLWIN_BOTTOM)
		{
			return WidgetScrollEvent::EvtBottom;
		}
		else if (eventType == wxEVT_SCROLLWIN_LINEUP)
		{
			return WidgetScrollEvent::EvtLineUp;
		}
		else if (eventType == wxEVT_SCROLLWIN_LINEDOWN)
		{
			return WidgetScrollEvent::EvtLineDown;
		}
		else if (eventType == wxEVT_SCROLLWIN_PAGEUP)
		{
			return WidgetScrollEvent::EvtPageUp;
		}
		else if (eventType == wxEVT_SCROLLWIN_PAGEDOWN)
		{
			return WidgetScrollEvent::EvtPageDown;
		}
		else if (eventType == wxEVT_SCROLLWIN_THUMBTRACK)
		{
			return WidgetScrollEvent::EvtThumbTrack;
		}
		else if (eventType == wxEVT_SCROLLWIN_THUMBRELEASE)
		{
			return WidgetScrollEvent::EvtThumbRelease;
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
	// wxEvtHandler
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

	// EvtHandlerWrapperBase
	bool EvtHandlerWrapperBase::TranslateTextEvent(IEvtHandler& evtHandler, wxEvent& event)
	{
		auto TranslateEvent = [&evtHandler, &event, eventType = event.GetEventType()](auto& object)
		{
			if (eventType == wxEVT_TEXT)
			{
				auto& commandEvent = static_cast<wxCommandEvent&>(event);
				return evtHandler.ProcessEvent(WidgetTextEvent::EvtChanged, object, commandEvent.GetString());
			}
			else if (eventType == wxEVT_TEXT_CUT)
			{
				auto& commandEvent = static_cast<wxCommandEvent&>(event);
				return evtHandler.ProcessEvent(WidgetTextEvent::EvtCut, object, commandEvent.GetString());
			}
			else if (eventType == wxEVT_TEXT_COPY)
			{
				auto& commandEvent = static_cast<wxCommandEvent&>(event);
				return evtHandler.ProcessEvent(WidgetTextEvent::EvtCopy, object, commandEvent.GetString());
			}
			else if (eventType == wxEVT_TEXT_PASTE)
			{
				auto& commandEvent = static_cast<wxCommandEvent&>(event);
				return evtHandler.ProcessEvent(WidgetTextEvent::EvtPaste, object, commandEvent.GetString());
			}
			else if (eventType == wxEVT_TEXT_ENTER)
			{
				return evtHandler.ProcessEvent(WidgetTextEvent::EvtCommit, object);
			}
			else if (eventType == wxEVT_TEXT_MAXLEN)
			{
				size_t limit = ITextEntry::npos;

				using T = std::remove_reference_t<decltype(object)>;
				if constexpr(std::is_same_v<T, ITextEntry> || std::is_same_v<T, ITextWidget>)
				{
					limit = object.GetLengthLimit();
				}
				return evtHandler.ProcessEvent(WidgetTextEvent::EvtLengthLimit, object, limit);
			}
			else if (eventType == wxEVT_TEXT_URL)
			{
				auto& urlEvent = static_cast<wxTextUrlEvent&>(event);
				if (auto widget = object.QueryInterface<IWidget>())
				{
					WidgetMouseEvent mouseEvent(*widget, urlEvent.GetMouseEvent());
					return evtHandler.ProcessEvent(WidgetTextEvent::EvtURI, object, urlEvent.GetString(), std::move(mouseEvent));
				}
				else
				{
					return evtHandler.ProcessEvent(WidgetTextEvent::EvtURI, object, urlEvent.GetString());
				}
			}
			return false;
		};

		if (auto textWidget = evtHandler.QueryInterface<ITextWidget>())
		{
			return TranslateEvent(*textWidget);
		}
		else if (auto textEntry = evtHandler.QueryInterface<ITextEntry>())
		{
			return TranslateEvent(*textEntry);
		}
		else if (auto widget = evtHandler.QueryInterface<IWidget>())
		{
			return TranslateEvent(*widget);
		}
		return false;
	}
	bool EvtHandlerWrapperBase::TranslateScrollEvent(IEvtHandler& evtHandler, wxEvent& event)
	{
		const auto eventType = event.GetEventType();
		if (auto eventID = MapScrollEventEvent(eventType))
		{
			auto& eventWx = static_cast<wxScrollEvent&>(event);

			WidgetScrollEvent event(m_Widget, eventWx.GetOrientation() == wxVERTICAL ? Orientation::Vertical : Orientation::Horizontal, eventWx.GetPosition());
			return m_Widget.ProcessEvent(event, eventID);
		}
		else if (auto eventID = MapScrollWinEventEvent(eventType))
		{
			auto& eventWx = static_cast<wxScrollWinEvent&>(event);

			WidgetScrollEvent event(m_Widget, eventWx.GetOrientation() == wxVERTICAL ? Orientation::Vertical : Orientation::Horizontal, eventWx.GetPosition());
			return m_Widget.ProcessEvent(event, eventID);
		}
		return false;
	}
}
