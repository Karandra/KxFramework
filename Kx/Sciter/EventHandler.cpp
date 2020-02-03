#include "KxStdAfx.h"
#include "EventHandler.h"
#include "SciterAPI.h"
#include "Internal.h"
#include "Host.h"
#include "KxFramework/KxUtility.h"

#pragma warning(disable: 4302) // 'reinterpret_cast': truncation from 'void *' to 'UINT'
#pragma warning(disable: 4311) // 'reinterpret_cast': pointer truncation from 'void *' to 'UINT'

namespace KxSciter
{
	template<class TEvent>
	TEvent MakeEvent(EventHandler& evtHandler, KxEventID eventID = wxEVT_NULL)
	{
		TEvent event(evtHandler.GetHost());
		event.Allow();
		event.SetEventObject(&evtHandler.GetEvtHandler());
		event.SetEventType(eventID);

		return event;
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
	ScrollSource MapScrollSource(SCROLL_SOURCE source)
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
	BOOL EventHandler::CallHostEventhandler(void* context, ElementHandle* element, uint32_t eventGroupID, void* parameters)
	{
		if (context)
		{
			return reinterpret_cast<EventHandler*>(context)->SciterHandleEvent(element, eventGroupID, parameters);
		}
		return FALSE;
	}

	int EventHandler::HandleLoadDataNotification(void* context)
	{
		SCN_LOAD_DATA& notification = *reinterpret_cast<SCN_LOAD_DATA*>(context);

		LPCBYTE pb = nullptr;
		UINT cb = 0;
		aux::wchars wu = aux::chars_of(notification.uri);

		if (wu.like(WSTR("res:*")))
		{
			if (sciter::load_resource_data(nullptr, wu.start + 4, pb, cb))
			{
				GetSciterAPI()->SciterDataReady(GetSciterHandle(), notification.uri, pb, cb);
			}
			return LOAD_DISCARD;
		}
		else if (wu.like(WSTR("this://app/*")))
		{
			// try to get them from archive first
			aux::bytes adata = sciter::archive::instance().get(wu.start+11);
			if (adata.length)
			{
				GetSciterAPI()->SciterDataReady(GetSciterHandle(), notification.uri, adata.start, adata.length);
			}
			return LOAD_DISCARD;
		}
		return LOAD_OK;
	}
	int EventHandler::HandleDataLoadedNotification(void* context)
	{
		SCN_DATA_LOADED& notification = *reinterpret_cast<SCN_DATA_LOADED*>(context);
		return 0;
	}
	int EventHandler::HandleAttachBehaviorNotification(void* context)
	{
		SCN_ATTACH_BEHAVIOR& notification = *reinterpret_cast<SCN_ATTACH_BEHAVIOR*>(context);
		return sciter::create_behavior(&notification);
	}
	int EventHandler::HandlePostedNotification(void* context)
	{
		SCN_POSTED_NOTIFICATION& notification = *reinterpret_cast<SCN_POSTED_NOTIFICATION*>(context);
		return 0;
	}
	int EventHandler::handleCriticalFailureNotification()
	{
		return 0;
	}
	int EventHandler::HandleDestroyedNotification()
	{
		return 0;
	}

	bool EventHandler::HandleInitializationEvent(ElementHandle* element, void* context)
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
	bool EventHandler::HandleKeyEvent(ElementHandle* element, void* context)
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
			event.SetPosition(m_Host.GetWindow().ScreenToClient(wxGetMousePosition()));
			AssignKeyboardState(event, parameters.alt_state);

			return ProcessEvent(event);
		}
		return false;
	}
	bool EventHandler::HandleMouseEvent(ElementHandle* element, void* context)
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
	bool EventHandler::HandleFocusEvent(ElementHandle* element, void* context)
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
	bool EventHandler::HandleSizeEvent(ElementHandle* element, void* context)
	{
		Event event = MakeEvent<SizeEvent>(*this, EvtSize);
		return ProcessEvent(event);
	}
	bool EventHandler::HandleTimerEvent(ElementHandle* element, void* context)
	{
		TIMER_PARAMS& parameters = *reinterpret_cast<TIMER_PARAMS*>(context);

		TimerEvent event = MakeEvent<TimerEvent>(*this, EvtTimer);
		event.SetTimerID(parameters.timerId);
		return ProcessEvent(event);
	}
	bool EventHandler::HandleScrollEvent(ElementHandle* element, void* context)
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
	bool EventHandler::HandleDrawEvent(ElementHandle* element, void* context)
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
	bool EventHandler::HandleBehaviorEvent(ElementHandle* element, void* context)
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

	WXHWND EventHandler::GetSciterHandle() const
	{
		return m_Host.GetWindow().GetHandle();
	}

	void EventHandler::AttachHost()
	{
		GetSciterAPI()->SciterWindowAttachEventHandler(GetSciterHandle(), reinterpret_cast<LPELEMENT_EVENT_PROC>(CallHostEventhandler), this, HANDLE_ALL);
	}
	void EventHandler::DetachHost()
	{
		GetSciterAPI()->SciterWindowDetachEventHandler(GetSciterHandle(), reinterpret_cast<LPELEMENT_EVENT_PROC>(CallHostEventhandler), this);
	}
	void EventHandler::AttachElement(Element& element)
	{
		GetSciterAPI()->SciterAttachEventHandler(ToSciterElement(element.GetHandle()), reinterpret_cast<LPELEMENT_EVENT_PROC>(CallHostEventhandler), this);
	}
	void EventHandler::DetachElement(Element& element)
	{
		GetSciterAPI()->SciterDetachEventHandler(ToSciterElement(element.GetHandle()), reinterpret_cast<LPELEMENT_EVENT_PROC>(CallHostEventhandler), this);
	}

	bool EventHandler::SciterHandleEvent(ElementHandle* element, uint32_t eventGroupID, void* context)
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
	int EventHandler::SciterHandleNotify(void* context)
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

	bool EventHandler::ProcessEvent(wxEvent& event)
	{
		return GetEvtHandler().ProcessEvent(event) && !event.GetSkipped();
	}
}
