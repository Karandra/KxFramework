#include "KxStdAfx.h"
#include "Host.h"
#include "SciterAPI.h"
#include "Internal.h"
#include "KxFramework/KxUtility.h"

#pragma warning(disable: 4302) // 'reinterpret_cast': truncation from 'void *' to 'UINT'
#pragma warning(disable: 4311) // 'reinterpret_cast': pointer truncation from 'void *' to 'UINT'

namespace
{
	template<class TEvent>
	TEvent MakeEvent(KxSciter::Host& host, KxEventID eventID = wxEVT_NULL)
	{
		TEvent event(host);
		event.Allow();
		event.SetEventObject(&host.GetWindow());
		event.SetEventType(eventID);

		return event;
	}

	KxSciter::ElementHandle* FromSciterElement(HELEMENT handle)
	{
		return reinterpret_cast<KxSciter::ElementHandle*>(handle);
	}

	void AssignKeyboardState(wxKeyboardState& state, UINT sciterValues)
	{
		state.SetAltDown(sciterValues & KEYBOARD_STATES::ALT_KEY_PRESSED);
		state.SetShiftDown(sciterValues & KEYBOARD_STATES::SHIFT_KEY_PRESSED);
		state.SetControlDown(sciterValues & KEYBOARD_STATES::CONTROL_KEY_PRESSED);
		state.SetMetaDown(wxGetKeyState(wxKeyCode::WXK_WINDOWS_LEFT) || wxGetKeyState(wxKeyCode::WXK_WINDOWS_RIGHT));
	}
	void AssignMouseState(wxMouseState& state, UINT sciterValues)
	{
		state.SetLeftDown(sciterValues & MOUSE_BUTTONS::MAIN_MOUSE_BUTTON);
		state.SetRightDown(sciterValues & MOUSE_BUTTONS::PROP_MOUSE_BUTTON);
		state.SetMiddleDown(sciterValues & MOUSE_BUTTONS::MIDDLE_MOUSE_BUTTON);
		state.SetAux1Down(::GetKeyState(VK_XBUTTON1) < 0);
		state.SetAux2Down(::GetKeyState(VK_XBUTTON2) < 0);
	}
	wxStockCursor MapCursorType(CURSOR_TYPE type)
	{
		switch (type)
		{
			case CURSOR_TYPE::CURSOR_ARROW:
			case CURSOR_TYPE::CURSOR_UPARROW:
			case CURSOR_TYPE::CURSOR_DRAG_MOVE:
			case CURSOR_TYPE::CURSOR_DRAG_COPY:
			{
				return wxStockCursor::wxCURSOR_ARROW;
			}
			case CURSOR_TYPE::CURSOR_IBEAM:
			{
				return wxStockCursor::wxCURSOR_IBEAM;
			}
			case CURSOR_TYPE::CURSOR_WAIT:
			{
				return wxStockCursor::wxCURSOR_WAIT;
			}
			case CURSOR_TYPE::CURSOR_CROSS:
			{
				return wxStockCursor::wxCURSOR_CROSS;
			}
			case CURSOR_TYPE::CURSOR_SIZENWSE:
			{
				return wxStockCursor::wxCURSOR_SIZENWSE;
			}
			case CURSOR_TYPE::CURSOR_SIZENESW:
			{
				return wxStockCursor::wxCURSOR_SIZENESW;
			}
			case CURSOR_TYPE::CURSOR_SIZEWE:
			{
				return wxStockCursor::wxCURSOR_SIZEWE;
			}
			case CURSOR_TYPE::CURSOR_SIZENS:
			{
				return wxStockCursor::wxCURSOR_SIZENS;
			}
			case CURSOR_TYPE::CURSOR_SIZEALL:
			{
				return wxStockCursor::wxCURSOR_SIZING;
			}
			case CURSOR_TYPE::CURSOR_NO:
			{
				return wxStockCursor::wxCURSOR_WAIT;
			}
			case CURSOR_TYPE::CURSOR_APPSTARTING:
			{
				return wxStockCursor::wxCURSOR_ARROWWAIT;
			}
			case CURSOR_TYPE::CURSOR_HELP:
			{
				return wxStockCursor::wxCURSOR_QUESTION_ARROW;
			}
			case CURSOR_TYPE::CURSOR_HAND:
			{
				return wxStockCursor::wxCURSOR_HAND;
			}
		}
		return wxStockCursor::wxCURSOR_NONE;
	}
	KxSciter::ScrollSource MapScrollSource(SCROLL_SOURCE source)
	{
		using namespace KxSciter;

		switch (source)
		{
			case SCROLL_SOURCE::SCROLL_SOURCE_KEYBOARD:
			{
				return ScrollSource::Keyboard;
			}
			case SCROLL_SOURCE::SCROLL_SOURCE_SCROLLBAR:
			{
				return ScrollSource::Scrollbar;
			}
			case SCROLL_SOURCE::SCROLL_SOURCE_ANIMATOR:
			{
				return ScrollSource::Animator;
			}
			case SCROLL_SOURCE::SCROLL_SOURCE_WHEEL:
			{
				return ScrollSource::Wheel;
			}
		};
		return ScrollSource::Unknown;
	}
}

namespace KxSciter
{
	int Host::HandleLoadDataNotification(void* context)
	{
		SCN_LOAD_DATA& notification = *reinterpret_cast<SCN_LOAD_DATA*>(context);

		LPCBYTE pb = nullptr;
		UINT cb = 0;
		aux::wchars wu = aux::chars_of(notification.uri);

		if (wu.like(WSTR("res:*")))
		{
			if (sciter::load_resource_data(nullptr, wu.start + 4, pb, cb))
			{
				GetSciterAPI()->SciterDataReady(m_SciterWindow.GetHandle(), notification.uri, pb, cb);
			}
			return LOAD_DISCARD;
		}
		else if (wu.like(WSTR("this://app/*")))
		{
			// try to get them from archive first
			aux::bytes adata = sciter::archive::instance().get(wu.start+11);
			if (adata.length)
			{
				GetSciterAPI()->SciterDataReady(m_SciterWindow.GetHandle(), notification.uri, adata.start, adata.length);
			}
			return LOAD_DISCARD;
		}
		return LOAD_OK;
	}
	int Host::HandleDataLoadedNotification(void* context)
	{
		SCN_DATA_LOADED& notification = *reinterpret_cast<SCN_DATA_LOADED*>(context);
		return 0;
	}
	int Host::HandleAttachBehaviorNotification(void* context)
	{
		SCN_ATTACH_BEHAVIOR& notification = *reinterpret_cast<SCN_ATTACH_BEHAVIOR*>(context);
		return sciter::create_behavior(&notification);
	}
	int Host::HandlePostedNotification(void* context)
	{
		SCN_POSTED_NOTIFICATION& notification = *reinterpret_cast<SCN_POSTED_NOTIFICATION*>(context);
		return 0;
	}
	int Host::handleCriticalFailureNotification()
	{
		return 0;
	}
	int Host::HandleDestroyedNotification()
	{
		return 0;
	}

	bool Host::HandleInitializationEvent(ElementHandle* element, void* context)
	{
		INITIALIZATION_PARAMS& parameters = *reinterpret_cast<INITIALIZATION_PARAMS*>(context);

		if (parameters.cmd == BEHAVIOR_ATTACH)
		{
			Event event = MakeEvent<Event>(*this, EvtAttached);
			event.SetElement(element);
			return ProcessEvent(event);
		}
		else if (parameters.cmd == BEHAVIOR_DETACH)
		{
			Event event = MakeEvent<Event>(*this, EvtDetached);
			event.SetElement(element);
			return ProcessEvent(event);
		}
		return true;
	}
	bool Host::HandleKeyEvent(ElementHandle* element, void* context)
	{
		KEY_PARAMS& parameters = *reinterpret_cast<KEY_PARAMS*>(context);

		KeyEvent event = MakeEvent<KeyEvent>(*this);
		switch (parameters.cmd)
		{
			case KEY_EVENTS::KEY_CHAR:
			{
				event.SetEventType(EvtKeyChar);
				event.SetUnicodeKey(parameters.key_code);
				break;
			}
			case KEY_EVENTS::KEY_UP:
			{
				event.SetEventType(EvtKeyUp);
				break;
			}
			case KEY_EVENTS::KEY_DOWN:
			{
				event.SetEventType(EvtKeyDown);
				break;
			}
		};

		if (event.GetEventType() != wxEVT_NULL)
		{
			event.SetElement(element);
			event.SetTargetElement(FromSciterElement(parameters.target));
			event.SetKeyCode(static_cast<wxKeyCode>(parameters.key_code));
			event.SetPosition(m_SciterWindow.ScreenToClient(wxGetMousePosition()));
			AssignKeyboardState(event, parameters.alt_state);

			return ProcessEvent(event);
		}
		return false;
	}
	bool Host::HandleMouseEvent(ElementHandle* element, void* context)
	{
		MOUSE_PARAMS& parameters = *reinterpret_cast<MOUSE_PARAMS*>(context);

		MouseEvent event = MakeEvent<MouseEvent>(*this);
		switch (parameters.cmd)
		{
			case MOUSE_EVENTS::MOUSE_ENTER:
			{
				event.SetEventType(EvtMouseEnter);
				break;
			}
			case MOUSE_EVENTS::MOUSE_LEAVE:
			{
				event.SetEventType(EvtMouseLeave);
				break;
			}
			case MOUSE_EVENTS::MOUSE_MOVE:
			{
				event.SetEventType(EvtMouseMove);
				break;
			}
			case MOUSE_EVENTS::MOUSE_UP:
			{
				event.SetEventType(EvtMouseUp);
				break;
			}
			case MOUSE_EVENTS::MOUSE_DOWN:
			{
				event.SetEventType(EvtMouseDown);
				break;
			}
			case MOUSE_EVENTS::MOUSE_CLICK:
			{
				event.SetEventType(EvtMouseClick);
				break;
			}
			case MOUSE_EVENTS::MOUSE_DCLICK:
			{
				event.SetEventType(EvtMouseDoubleClick);
				break;
			}
			case MOUSE_EVENTS::MOUSE_TICK:
			{
				event.SetEventType(EvtMouseTick);
				break;
			}
			case MOUSE_EVENTS::MOUSE_IDLE:
			{
				event.SetEventType(EvtMouseIdle);
				break;
			}

			// Skipping drag events for now. Sciter also has "exchange" event and it seems also have something to do with drag and drop.
			// Further investigation required.
		};

		if (event.GetEventType() != wxEVT_NULL)
		{
			event.SetElement(element);
			event.SetTargetElement(FromSciterElement(parameters.target));
			event.SetPosition({parameters.pos_view.x, parameters.pos_view.y});
			event.SetRelativePosition({parameters.pos.x, parameters.pos.y});
			event.SetIsOnIcon(parameters.is_on_icon);
			event.SetCursorType(MapCursorType(static_cast<CURSOR_TYPE>(parameters.cursor_type)));
			AssignKeyboardState(event, parameters.alt_state);
			AssignMouseState(event, parameters.button_state);

			return ProcessEvent(event);
		}
		return false;
	}
	bool Host::HandleFocusEvent(ElementHandle* element, void* context)
	{
		FOCUS_PARAMS& parameters = *reinterpret_cast<FOCUS_PARAMS*>(context);

		FocusEvent event = MakeEvent<FocusEvent>(*this);
		switch (parameters.cmd)
		{
			case FOCUS_EVENTS::FOCUS_GOT:
			{
				event.SetEventType(EvtSetFocus);
				break;
			}
			case FOCUS_EVENTS::FOCUS_LOST:
			{
				event.SetEventType(EvtKillFocus);
				break;
			}
			case FOCUS_EVENTS::FOCUS_IN:
			{
				event.SetEventType(EvtContainerSetFocus);
				break;
			}
			case FOCUS_EVENTS::FOCUS_OUT:
			{
				event.SetEventType(EvtContainerKillFocus);
				break;
			}
		};

		if (event.GetEventType() != wxEVT_NULL)
		{
			event.SetElement(element);
			event.SetTargetElement(FromSciterElement(parameters.target));
			event.SetByMouseClick(parameters.by_mouse_click);

			const bool result = ProcessEvent(event);
			parameters.cancel = !event.IsAllowed();
			return result;
		}
		return false;
	}
	bool Host::HandleSizeEvent(ElementHandle* element, void* context)
	{
		Event event = MakeEvent<SizeEvent>(*this, EvtSize);
		return ProcessEvent(event);
	}
	bool Host::HandleTimerEvent(ElementHandle* element, void* context)
	{
		TIMER_PARAMS& parameters = *reinterpret_cast<TIMER_PARAMS*>(context);

		TimerEvent event = MakeEvent<TimerEvent>(*this, EvtTimer);
		event.SetTimerID(parameters.timerId);
		return ProcessEvent(event);
	}
	bool Host::HandleScrollEvent(ElementHandle* element, void* context)
	{
		SCROLL_PARAMS& parameters = *reinterpret_cast<SCROLL_PARAMS*>(context);

		ScrollEvent event = MakeEvent<ScrollEvent>(*this);
		switch (parameters.cmd)
		{
			case SCROLL_EVENTS::SCROLL_HOME:
			{
				event.SetEventType(EvtScrollHome);
				break;
			}
			case SCROLL_EVENTS::SCROLL_END:
			{
				event.SetEventType(EvtScrollEnd);
				break;
			}
			case SCROLL_EVENTS::SCROLL_STEP_PLUS:
			{
				event.SetEventType(EvtScrollStepPlus);
				break;
			}
			case SCROLL_EVENTS::SCROLL_STEP_MINUS:
			{
				event.SetEventType(EvtScrollStepMinus);
				break;
			}
			case SCROLL_EVENTS::SCROLL_PAGE_PLUS:
			{
				event.SetEventType(EvtScrollPagePlus);
				break;
			}
			case SCROLL_EVENTS::SCROLL_PAGE_MINUS:
			{
				event.SetEventType(EvtScrollPageMinus);
				break;
			}
			case SCROLL_EVENTS::SCROLL_SLIDER_PRESSED:
			{
				event.SetEventType(EvtScrollSliderPressed);
				break;
			}
			case SCROLL_EVENTS::SCROLL_SLIDER_RELEASED:
			{
				event.SetEventType(EvtScrollSliderReleased);
				break;
			}
			case SCROLL_EVENTS::SCROLL_CORNER_PRESSED:
			{
				event.SetEventType(EvtScrollCornerPressed);
				break;
			}
			case SCROLL_EVENTS::SCROLL_CORNER_RELEASED:
			{
				event.SetEventType(EvtScrollCornerReleased);
				break;
			}
		};

		if (event.GetEventType() != wxEVT_NULL)
		{
			event.SetElement(element);
			event.SetTargetElement(FromSciterElement(parameters.target));
			event.SetOrientation(parameters.vertical ? wxVERTICAL : wxHORIZONTAL);
			event.SetPosition(parameters.pos);
			event.SetSource(MapScrollSource(static_cast<SCROLL_SOURCE>(parameters.source)));

			return ProcessEvent(event);
		}
		return false;
	}
	bool Host::HandleDrawEvent(ElementHandle* element, void* context)
	{
		DRAW_PARAMS& parameters = *reinterpret_cast<DRAW_PARAMS*>(context);

		PaintEvent event = MakeEvent<PaintEvent>(*this);
		switch (parameters.cmd)
		{
			case DRAW_EVENTS::DRAW_BACKGROUND:
			{
				event.SetEventType(EvtPaintBackground);
				break;
			}
			case DRAW_EVENTS::DRAW_FOREGROUND:
			{
				event.SetEventType(EvtPaintForeground);
				break;
			}
			case DRAW_EVENTS::DRAW_OUTLINE:
			{
				event.SetEventType(EvtPaintOutline);
				break;
			}
			case DRAW_EVENTS::DRAW_CONTENT:
			{
				event.SetEventType(EvtPaintContent);
				break;
			}
		};

		if (event.GetEventType() != wxEVT_NULL)
		{
			event.SetElement(element);
			event.SetDC(parameters.gfx);
			event.SetRect(KxUtility::CopyRECTToRect(parameters.area));

			return ProcessEvent(event);
		}
		return false;
	}
	bool Host::HandleBehaviorEvent(ElementHandle* element, void* context)
	{
		BEHAVIOR_EVENT_PARAMS& parameters = *reinterpret_cast<BEHAVIOR_EVENT_PARAMS*>(context);

		BehaviorEvent event = MakeEvent<BehaviorEvent>(*this);
		switch (parameters.cmd)
		{
			// Button
			case BEHAVIOR_EVENTS::BUTTON_CLICK:
			{
				event.SetEventType(BehaviorEvent::EvtButtonClick);
				break;
			}
			case BEHAVIOR_EVENTS::BUTTON_PRESS:
			{
				event.SetEventType(BehaviorEvent::EvtButtonPress);
				break;
			}
			case BEHAVIOR_EVENTS::BUTTON_STATE_CHANGED:
			{
				event.SetEventType(BehaviorEvent::EvtButtonStateChanged);
				break;
			}

			// Edit
			case BEHAVIOR_EVENTS::EDIT_VALUE_CHANGING:
			{
				event.SetEventType(BehaviorEvent::EvtEditValueChanging);
				break;
			}
			case BEHAVIOR_EVENTS::EDIT_VALUE_CHANGED:
			{
				event.SetEventType(BehaviorEvent::EvtEditValueChanged);
				break;
			}

			// Select
			case BEHAVIOR_EVENTS::SELECT_SELECTION_CHANGED:
			{
				event.SetEventType(BehaviorEvent::EvtSelectValueChanged);
				break;
			}
			case BEHAVIOR_EVENTS::SELECT_STATE_CHANGED:
			{
				event.SetEventType(BehaviorEvent::EvtSelectStateChanged);
				break;
			}

			// Popup
			case BEHAVIOR_EVENTS::POPUP_READY:
			{
				event.SetEventType(BehaviorEvent::EvtPopupReady);
				break;
			}
			case BEHAVIOR_EVENTS::POPUP_DISMISSING:
			{
				event.SetEventType(BehaviorEvent::EvtPopupDismissing);
				break;
			}
			case BEHAVIOR_EVENTS::POPUP_DISMISSED:
			{
				event.SetEventType(BehaviorEvent::EvtPopupDismissed);
				break;
			}
			case BEHAVIOR_EVENTS::POPUP_REQUEST:
			{
				event.SetEventType(BehaviorEvent::EvtPopupShowRequest);
				break;
			}
			case BEHAVIOR_EVENTS::CLOSE_POPUP:
			{
				event.SetEventType(BehaviorEvent::EvtPopupCloseRequest);
				break;
			}

			// Menu
			case BEHAVIOR_EVENTS::CONTEXT_MENU_REQUEST:
			{
				event.SetEventType(BehaviorEvent::EvtContextMenuRequest);
				break;
			}
			case BEHAVIOR_EVENTS::MENU_ITEM_ACTIVE:
			{
				event.SetEventType(BehaviorEvent::EvtMenuItemActive);
				break;
			}
			case BEHAVIOR_EVENTS::MENU_ITEM_CLICK:
			{
				event.SetEventType(BehaviorEvent::EvtMenuItemClick);
				break;
			}
			
			// History
			case BEHAVIOR_EVENTS::HISTORY_PUSH:
			{
				event.SetEventType(BehaviorEvent::EvtHistoryPush);
				break;
			}
			case BEHAVIOR_EVENTS::HISTORY_DROP:
			{
				event.SetEventType(BehaviorEvent::EvtHistoryDrop);
				break;
			}
			case BEHAVIOR_EVENTS::HISTORY_PRIOR:
			{
				event.SetEventType(BehaviorEvent::EvtHistoryBackward);
				break;
			}
			case BEHAVIOR_EVENTS::HISTORY_NEXT:
			{
				event.SetEventType(BehaviorEvent::EvtHistoryForward);
				break;
			}
			case BEHAVIOR_EVENTS::HISTORY_STATE_CHANGED:
			{
				event.SetEventType(BehaviorEvent::EvtHistoryStateChanged);
				break;
			}

			// Document
			case BEHAVIOR_EVENTS::DOCUMENT_COMPLETE:
			{
				event.SetEventType(BehaviorEvent::EvtDocumentComplete);
				break;
			}
			case BEHAVIOR_EVENTS::DOCUMENT_CREATED:
			{
				event.SetEventType(BehaviorEvent::EvtDocumentCreated);
				break;
			}
			case BEHAVIOR_EVENTS::DOCUMENT_PARSED:
			{
				event.SetEventType(BehaviorEvent::EvtDocumentParsed);
				break;
			}
			case BEHAVIOR_EVENTS::DOCUMENT_READY:
			{
				event.SetEventType(BehaviorEvent::EvtDocumentReady);
				break;
			}
			case BEHAVIOR_EVENTS::DOCUMENT_CLOSE_REQUEST:
			{
				event.SetEventType(BehaviorEvent::EvtDocumentClosing);
				break;
			}
			case BEHAVIOR_EVENTS::DOCUMENT_CLOSE:
			{
				event.SetEventType(BehaviorEvent::EvtDocumentClosed);
				break;
			}

			// Video
			case BEHAVIOR_EVENTS::VIDEO_INITIALIZED:
			{
				event.SetEventType(BehaviorEvent::EvtVideoInitialized);
				break;
			}
			case BEHAVIOR_EVENTS::VIDEO_STARTED:
			{
				event.SetEventType(BehaviorEvent::EvtVideoStarted);
				break;
			}
			case BEHAVIOR_EVENTS::VIDEO_STOPPED:
			{
				event.SetEventType(BehaviorEvent::EvtVideoStopped);
				break;
			}
			case BEHAVIOR_EVENTS::VIDEO_BIND_RQ:
			{
				event.SetEventType(BehaviorEvent::EvtVideoBindingRequest);
				break;
			}

			// Pagination
			case BEHAVIOR_EVENTS::PAGINATION_STARTS:
			{
				event.SetEventType(BehaviorEvent::EvtPaginationStart);
				break;
			}
			case BEHAVIOR_EVENTS::PAGINATION_PAGE:
			{
				event.SetEventType(BehaviorEvent::EvtPaginationPage);
				break;
			}
			case BEHAVIOR_EVENTS::PAGINATION_ENDS:
			{
				event.SetEventType(BehaviorEvent::EvtPaginationEnd);
				break;
			}

			// Generic
			case BEHAVIOR_EVENTS::CLICK:
			{
				event.SetEventType(BehaviorEvent::EvtGenericClick);
				break;
			}
			case BEHAVIOR_EVENTS::CHANGE:
			{
				event.SetEventType(BehaviorEvent::EvtGenericChange);
				break;
			}
			case BEHAVIOR_EVENTS::HYPERLINK_CLICK:
			{
				event.SetEventType(BehaviorEvent::EvtHyperlinkClick);
				break;
			}

			// Expand/Collapse
			case BEHAVIOR_EVENTS::ELEMENT_EXPANDED:
			{
				event.SetEventType(BehaviorEvent::EvtElementExpanded);
				break;
			}
			case BEHAVIOR_EVENTS::ELEMENT_COLLAPSED:
			{
				event.SetEventType(BehaviorEvent::EvtElementCollapsed);
				break;
			}

			// Forms
			case BEHAVIOR_EVENTS::FORM_SUBMIT:
			{
				event.SetEventType(BehaviorEvent::EvtFormSubmit);
				break;
			}
			case BEHAVIOR_EVENTS::FORM_RESET:
			{
				event.SetEventType(BehaviorEvent::EvtFormReset);
				break;
			}

			// Misc
			case BEHAVIOR_EVENTS::ANIMATION:
			{
				event.SetEventType(BehaviorEvent::EvtAnimation);
				break;
			}
			case BEHAVIOR_EVENTS::ACTIVATE_CHILD:
			{
				event.SetEventType(BehaviorEvent::EvtActivateChild);
				break;
			}

			case BEHAVIOR_EVENTS::VISIUAL_STATUS_CHANGED:
			{
				event.SetEventType(BehaviorEvent::EvtAnimation);
				break;
			}
			case BEHAVIOR_EVENTS::DISABLED_STATUS_CHANGED:
			{
				event.SetEventType(BehaviorEvent::EvtDisabledStatusChanged);
				break;
			}
			case BEHAVIOR_EVENTS::CONTENT_CHANGED:
			{
				event.SetEventType(BehaviorEvent::EvtContentChanged);
				break;
			}
			case BEHAVIOR_EVENTS::UI_STATE_CHANGED:
			{
				event.SetEventType(BehaviorEvent::EvtUIStateChanged);
				break;
			}
		};

		if (event.GetEventType() != wxEVT_NULL)
		{
			event.SetElement(element);
			event.SetSourceElement(FromSciterElement(parameters.he));
			event.SetTargetElement(FromSciterElement(parameters.heTarget));
			event.SetEventName(parameters.name);

			// "cancel"
			const bool processed = ProcessEvent(event);
			if (!event.IsAllowed())
			{
				// This event can be canceled this way
				if (parameters.cmd == DOCUMENT_CLOSE_REQUEST)
				{
					constexpr wchar_t cancelString[] = L"cancel";
					GetSciterAPI()->ValueInit(&parameters.data);
					GetSciterAPI()->ValueStringDataSet(&parameters.data, cancelString, std::size(cancelString) - 1, UT_STRING_SYMBOL);
				}
			}
			return processed;
		}
		return false;
	}

	void Host::SetDefaultOptions()
	{
		EnableSystemTheme();
		EnableSmoothScrolling();
		SetFontSmoothingMode(FontSmoothing::SystemDefault);
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_HTTPS_ERROR, 1);
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_CONNECTION_TIMEOUT, 500);
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_TRANSPARENT_WINDOW, true);
		GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SET_SCRIPT_RUNTIME_FEATURES, ALLOW_FILE_IO|ALLOW_SOCKET_IO|ALLOW_EVAL|ALLOW_SYSINFO);
	}
	void Host::SetupCallbacks()
	{
		GetSciterAPI()->SciterSetCallback(m_SciterWindow.GetHandle(), [](SCITER_CALLBACK_NOTIFICATION* notification, void* context) -> UINT
		{
			if (notification && context)
			{
				return reinterpret_cast<Host*>(context)->SciterHandleNotify(notification);
			}
			return 0;
		}, this);
		GetSciterAPI()->SciterWindowAttachEventHandler(m_SciterWindow.GetHandle(), [](void* context, HELEMENT element, UINT eventGroupID, void* parameters) -> BOOL
		{
			if (context)
			{
				return reinterpret_cast<Host*>(context)->SciterHandleEvent(reinterpret_cast<ElementHandle*>(element), eventGroupID, parameters);
			}
			return FALSE;
		}, this, HANDLE_ALL);
	}

	void Host::OnEngineCreated()
	{
		// Sciter options
		SetDefaultOptions();
		SetupCallbacks();

		// Window options
		m_SciterWindow.SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);

		// Send event
		Event event = MakeEvent<Event>(*this, EvtEngineCreated);
		ProcessEvent(event);
	}
	void Host::OnEngineDestroyed()
	{
		Event event = MakeEvent<Event>(*this, EvtEngineDestroyed);
		ProcessEvent(event);
	}
	bool Host::ProcessEvent(wxEvent& event)
	{
		return m_SciterWindow.ProcessWindowEvent(event) && !event.GetSkipped();
	}

	bool Host::SciterHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam)
	{
		if (m_AllowSciterHandleMessage)
		{
			// Forward message to Sciter
			BOOL handled = FALSE;
			*result = GetSciterAPI()->SciterProcND(m_SciterWindow.GetHandle(), msg, wParam, lParam, &handled);

			// Handle engine creation callbacks
			if (msg == WM_CREATE)
			{
				::SetWindowLongPtrW(m_SciterWindow.GetHandle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
				m_EngineCreated = true;

				OnEngineCreated();
			}
			else if (msg == WM_DESTROY)
			{
				OnEngineDestroyed();
				m_EngineCreated = false;
				m_AllowSciterHandleMessage = false;

				::SetWindowLongPtrW(m_SciterWindow.GetHandle(), GWLP_USERDATA, 0);
			}
			return handled;
		}
		return false;
	}
	bool Host::SciterHandleEvent(ElementHandle* element, uint32_t eventGroupID, void* context)
	{
		switch (eventGroupID)
		{
			case EVENT_GROUPS::SUBSCRIPTIONS_REQUEST:
			{
				// Defines list of event groups this event handler is subscribed to
				*reinterpret_cast<UINT*>(context) = HANDLE_ALL;
				return true;
			}
			case EVENT_GROUPS::HANDLE_INITIALIZATION:
			{
				return HandleInitializationEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_KEY:
			{
				return HandleKeyEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_MOUSE:
			{
				return HandleMouseEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_FOCUS:
			{
				return HandleFocusEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_SIZE:
			{
				return HandleSizeEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_TIMER:
			{
				return HandleTimerEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_SCROLL:
			{
				return HandleScrollEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_DRAW:
			{
				return HandleDrawEvent(element, context);
			}
			case EVENT_GROUPS::HANDLE_BEHAVIOR_EVENT:
			{
				return HandleBehaviorEvent(element, context);
			}
		};
		return false;
	}
	int Host::SciterHandleNotify(void* context)
	{
		SCITER_CALLBACK_NOTIFICATION& notification = *reinterpret_cast<SCITER_CALLBACK_NOTIFICATION*>(context);

		switch (notification.code)
		{
			case SC_LOAD_DATA:
			{
				return HandleLoadDataNotification(context);
			}
			case SC_DATA_LOADED:
			{
				return HandleDataLoadedNotification(context);
			}
			case SC_ATTACH_BEHAVIOR:
			{
				return HandleAttachBehaviorNotification(context);
			}
			case SC_POSTED_NOTIFICATION:
			{
				return HandlePostedNotification(context);
			}
			case SC_GRAPHICS_CRITICAL_FAILURE:
			{
				return handleCriticalFailureNotification();
			}
			case SC_ENGINE_DESTROYED:
			{
				return HandleDestroyedNotification();
			}
		};
		return 0;
	}
	void Host::OnInternalIdle()
	{
		if (m_ReloadScheduled)
		{
			if (auto root = GetRootElement())
			{
				LoadHTML(root.GetOuterHTML(), m_DocumentBasePath);
			}
			m_ReloadScheduled = false;
		}
	}

	Host::Host(wxWindow& window)
		:m_SciterWindow(window)
	{
		if (m_SciterWindow.GetHandle())
		{
			::SetWindowLongPtrW(m_SciterWindow.GetHandle(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		}

		// Child windows are fine with already created window. They don't need 'WS_EX_NOREDIRECTIONBITMAP' style
		if (!m_SciterWindow.IsTopLevel())
		{
			m_AllowSciterHandleMessage = true;
		}
	}
	Host::~Host()
	{
	}

	bool Host::Create()
	{
		if (m_SciterWindow.IsTopLevel())
		{
			if (m_EngineCreated)
			{
				return false;
			}

			// Get original window info
			const long style = m_SciterWindow.GetWindowStyle();
			const wxChar* nativeClassName = m_SciterWindow.GetMSWClassName(style);

			WXDWORD nativeExStyle = 0;
			WXDWORD nativeStyle = m_SciterWindow.MSWGetStyle(style, &nativeExStyle);

			const wxPoint pos = m_SciterWindow.GetPosition();
			const wxSize size = m_SciterWindow.GetSize();
			const wxString title = m_SciterWindow.GetLabel();

			// Destroy and detach original window
			const HWND oldHandle = m_SciterWindow.GetHandle();
			m_SciterWindow.DestroyChildren();
			m_SciterWindow.DissociateHandle();
			::DestroyWindow(oldHandle);

			// Create new window with 'WS_EX_NOREDIRECTIONBITMAP' extended style instead and attach it to the wxWindow
			m_AllowSciterHandleMessage = true;
			m_SciterWindow.MSWCreate(nativeClassName, title.wc_str(), pos, size, nativeStyle, nativeExStyle|WS_EX_NOREDIRECTIONBITMAP);
			return m_EngineCreated;
		}
		return true;
	}
	void Host::Update()
	{
		GetSciterAPI()->SciterUpdateWindow(m_SciterWindow.GetHandle());
	}
	void Host::Reload()
	{
		m_ReloadScheduled = true;
	}

	wxSize Host::GetBestSize() const
	{
		const int paddingX = wxSystemSettings::GetMetric(wxSYS_SMALLICON_X, &m_SciterWindow);
		const int paddingY = wxSystemSettings::GetMetric(wxSYS_SMALLICON_Y, &m_SciterWindow);

		int width = GetSciterAPI()->SciterGetMinWidth(m_SciterWindow.GetHandle()) + paddingX;
		int height = GetSciterAPI()->SciterGetMinHeight(m_SciterWindow.GetHandle(), width) + paddingY;

		return m_SciterWindow.FromDIP(wxSize(width, height));
	}
	wxSize Host::GetDPI() const
	{
		UINT x = 0;
		UINT y = 0;
		GetSciterAPI()->SciterGetPPI(m_SciterWindow.GetHandle(), &x, &y);

		return wxSize(x, y);
	}

	bool Host::IsSystemThemeEnabled() const
	{
		return m_Option_ThemeEnabled;
	}
	bool Host::EnableSystemTheme(bool enable)
	{
		m_Option_ThemeEnabled = enable;
		return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SET_UX_THEMING, enable);
	}

	bool Host::IsSmoothScrollingEnabled() const
	{
		return m_Option_SmoothScrolling;
	}
	bool Host::EnableSmoothScrolling(bool enable)
	{
		m_Option_SmoothScrolling = enable;
		return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_SMOOTH_SCROLL, enable);
	}

	FontSmoothing Host::GetFontSommthingMode() const
	{
		return m_Option_FontSmoothing;
	}
	bool Host::SetFontSmoothingMode(FontSmoothing mode)
	{
		switch (mode)
		{
			case FontSmoothing::None:
			{
				return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 1);
			}
			case FontSmoothing::SystemDefault:
			{
				return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 0);
			}
			case FontSmoothing::Standard:
			{
				return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 2);
			}
			case FontSmoothing::ClearType:
			{
				return GetSciterAPI()->SciterSetOption(m_SciterWindow.GetHandle(), SCITER_FONT_SMOOTHING, 3);
			}
		};
		return false;
	}

	bool Host::SetWindowFrame(WindowFrame windowFrame)
	{
		if (!m_SciterWindow.IsTopLevel())
		{
			return false;
		}

		auto SetAttribute = [&](const wxString& value)
		{
			if (GetRootElement().SetAttribute(wxS("window-frame"), value))
			{
				Reload();
				return true;
			}
			return false;
		};

		switch (windowFrame)
		{
			case WindowFrame::Standard:
			{
				return SetAttribute(wxS("none"));
			}
			case WindowFrame::Solid:
			{
				return SetAttribute(wxS("solid"));
			}
			case WindowFrame::SolidWithShadow:
			{
				return SetAttribute(wxS("solid-with-shadow"));
			}
			case WindowFrame::Extended:
			{
				return SetAttribute(wxS("extended"));
			}
			case WindowFrame::Transparent:
			{
				return SetAttribute(wxS("transparent"));
			}
		};
		return false;
	}
	bool Host::SetBlurBehind(WindowBlurBehind blurMode)
	{
		if (!m_SciterWindow.IsTopLevel())
		{
			return false;
		}

		auto SetAttribute = [&](const wxString& value)
		{
			GetRootElement().SetAttribute(wxS("window-blurbehind"), value);

			if (!value.IsEmpty())
			{
				return ExecuteScript(KxString::Format(wxS("view.windowBlurbehind = #%1;"), value));
			}
			return ExecuteScript(wxS("view.windowBlurbehind = undefined;"));
		};

		switch (blurMode)
		{
			case WindowBlurBehind::None:
			{
				return SetAttribute(wxS(""));
			}
			case WindowBlurBehind::Auto:
			{
				return SetAttribute(wxS("auto"));
			}
			case WindowBlurBehind::Light:
			{
				return SetAttribute(wxS("light"));
			}
			case WindowBlurBehind::UltraLight:
			{
				return SetAttribute(wxS("ultra-light"));
			}
			case WindowBlurBehind::Dark:
			{
				return SetAttribute(wxS("dark"));
			}
			case WindowBlurBehind::UltraDark:
			{
				return SetAttribute(wxS("ultra-dark"));
			}
		};
		return false;
	}

	bool Host::IsTransparentBackgroundSupported(wxString* reason) const
	{
		if (!m_SciterWindow.IsTopLevel())
		{
			if (reason)
			{
				*reason = "Not a top-level window";
			}
			return false;
		}
		return true;
	}
	bool Host::SetTransparency(uint8_t alpha)
	{
		return false;
	}

	bool Host::SetFont(const wxFont& font)
	{
		return GetRootElement().SetStyleFont(font);
	}
	bool Host::SetBackgroundColor(const KxColor& color)
	{
		return GetRootElement().SetStyleAttribute(wxS("background-color"), color);
	}
	bool Host::SetForegroundColor(const KxColor& color)
	{
		return GetRootElement().SetStyleAttribute(wxS("color"), color);
	}

	wxLayoutDirection Host::GetLayoutDirection() const
	{
		wxString value = GetRootElement().GetStyleAttribute(wxS("direction"));
		if (value == wxS("ltr"))
		{
			return wxLayoutDirection::wxLayout_LeftToRight;
		}
		else if (value == wxS("rtl"))
		{
			return wxLayoutDirection::wxLayout_RightToLeft;
		}
		return wxLayoutDirection::wxLayout_Default;
	}
	void Host::SetLayoutDirection(wxLayoutDirection value)
	{
		switch (value)
		{
			case wxLayoutDirection::wxLayout_LeftToRight:
			{
				GetRootElement().RemoveStyleAttribute(wxS("ltr"));
				break;
			}
			case wxLayoutDirection::wxLayout_RightToLeft:
			{
				GetRootElement().RemoveStyleAttribute(wxS("rtl"));
				break;
			}
			default:
			{
				GetRootElement().RemoveStyleAttribute(wxS("direction"));
			}
		};
	}

	bool Host::LoadHTML(const wxString& html, const wxString& basePath)
	{
		m_DocumentPath.clear();
		m_DocumentBasePath = basePath;
		if (!m_DocumentBasePath.IsEmpty() && !m_DocumentBasePath.StartsWith(wxS("file://")))
		{
			m_DocumentBasePath.Prepend(wxS("file://"));
		}
		if (!m_DocumentBasePath.IsEmpty() && m_DocumentBasePath.Last() != wxS('\\'))
		{
			m_DocumentBasePath += wxS('\\');
		}

		auto utf8 = Internal::ToSciterUTF8(html);
		return GetSciterAPI()->SciterLoadHtml(m_SciterWindow.GetHandle(), utf8.data(), utf8.size(), m_DocumentBasePath.wc_str());
	}
	bool Host::LoadHTML(const wxString& html, const KxURI& baseURI)
	{
		m_DocumentPath.clear();
		m_DocumentBasePath = baseURI.BuildURI();
		
		auto utf8 = Internal::ToSciterUTF8(html);
		return GetSciterAPI()->SciterLoadHtml(m_SciterWindow.GetHandle(), utf8.data(), utf8.size(), m_DocumentBasePath.wc_str());
	}

	bool Host::LoadDocument(const wxString& localPath)
	{
		m_DocumentPath = localPath;
		m_DocumentBasePath = localPath.BeforeLast(wxS('\\'));
		return GetSciterAPI()->SciterLoadFile(m_SciterWindow.GetHandle(), m_DocumentPath.wc_str());
	}
	bool Host::LoadDocument(const KxURI& uri)
	{
		m_DocumentPath = uri.BuildURI();
		m_DocumentBasePath = m_DocumentPath;
		return GetSciterAPI()->SciterLoadFile(m_SciterWindow.GetHandle(), m_DocumentBasePath.wc_str());
	}

	void Host::ClearDocument()
	{
		m_DocumentBasePath.clear();
		LoadHTML(wxEmptyString);
	}

	Element Host::GetRootElement() const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetRootElement(m_SciterWindow.GetHandle(), &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}
	Element Host::GetFocusedElement() const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetFocusElement(m_SciterWindow.GetHandle(), &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}
	Element Host::GetElementByUID(ElementUID* id) const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetElementByUID(m_SciterWindow.GetHandle(), reinterpret_cast<UINT>(id), &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}
	Element Host::GetElementFromPoint(const wxPoint& pos) const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterFindElement(m_SciterWindow.GetHandle(), {pos.x, pos.y}, &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}

	Element Host::GetHighlightedElement() const
	{
		HELEMENT node = nullptr;
		if (GetSciterAPI()->SciterGetHighlightedElement(m_SciterWindow.GetHandle(), &node) == SCDOM_OK)
		{
			return reinterpret_cast<ElementHandle*>(node);
		}
		return {};
	}
	void Host::SetHighlightedElement(const Element& node)
	{
		GetSciterAPI()->SciterSetHighlightedElement(m_SciterWindow.GetHandle(), (HELEMENT)node.GetHandle());
	}

	bool Host::ExecuteScript(const wxString& script)
	{
		VALUE returnValue = {};
		return GetSciterAPI()->SciterEval(m_SciterWindow.GetHandle(), script.wc_str(), script.length(), reinterpret_cast<SCITER_VALUE*>(&returnValue));
	}
}
