#pragma once
#include "Event.h"

namespace KxFramework::Sciter
{
	class KX_API BehaviorEvent: public Event
	{
		public:
			// Button
			KxEVENT_MEMBER(BehaviorEvent, ButtonClick);
			KxEVENT_MEMBER(BehaviorEvent, ButtonPress);
			KxEVENT_MEMBER(BehaviorEvent, ButtonStateChanged);

			// Edit
			KxEVENT_MEMBER(BehaviorEvent, EditValueChanging);
			KxEVENT_MEMBER(BehaviorEvent, EditValueChanged);

			// Select
			KxEVENT_MEMBER(BehaviorEvent, SelectValueChanged);
			KxEVENT_MEMBER(BehaviorEvent, SelectStateChanged);

			// Popup
			KxEVENT_MEMBER(BehaviorEvent, PopupReady);
			KxEVENT_MEMBER(BehaviorEvent, PopupDismissing);
			KxEVENT_MEMBER(BehaviorEvent, PopupDismissed);
			KxEVENT_MEMBER(BehaviorEvent, PopupShowRequest);
			KxEVENT_MEMBER(BehaviorEvent, PopupCloseRequest);

			// Menu
			KxEVENT_MEMBER(BehaviorEvent, ContextMenuRequest);
			KxEVENT_MEMBER(BehaviorEvent, MenuItemActive);
			KxEVENT_MEMBER(BehaviorEvent, MenuItemClick);
			
			// History
			KxEVENT_MEMBER(BehaviorEvent, HistoryPush);
			KxEVENT_MEMBER(BehaviorEvent, HistoryDrop);
			KxEVENT_MEMBER(BehaviorEvent, HistoryBackward);
			KxEVENT_MEMBER(BehaviorEvent, HistoryForward);
			KxEVENT_MEMBER(BehaviorEvent, HistoryStateChanged);

			// Document
			KxEVENT_MEMBER(BehaviorEvent, DocumentComplete);
			KxEVENT_MEMBER(BehaviorEvent, DocumentCreated);
			KxEVENT_MEMBER(BehaviorEvent, DocumentParsed);
			KxEVENT_MEMBER(BehaviorEvent, DocumentReady);
			KxEVENT_MEMBER(BehaviorEvent, DocumentClosing);
			KxEVENT_MEMBER(BehaviorEvent, DocumentClosed);

			// Video
			KxEVENT_MEMBER(BehaviorEvent, VideoInitialized);
			KxEVENT_MEMBER(BehaviorEvent, VideoStarted);
			KxEVENT_MEMBER(BehaviorEvent, VideoStopped);
			KxEVENT_MEMBER(BehaviorEvent, VideoBindingRequest);

			// Pagination
			KxEVENT_MEMBER(BehaviorEvent, PaginationStart);
			KxEVENT_MEMBER(BehaviorEvent, PaginationPage);
			KxEVENT_MEMBER(BehaviorEvent, PaginationEnd);

			// Generic
			KxEVENT_MEMBER(BehaviorEvent, GenericClick);
			KxEVENT_MEMBER(BehaviorEvent, GenericChange);
			KxEVENT_MEMBER(BehaviorEvent, HyperlinkClick);

			// Expand/Collapse
			KxEVENT_MEMBER(BehaviorEvent, ElementExpanded);
			KxEVENT_MEMBER(BehaviorEvent, ElementCollapsed);

			// Forms
			KxEVENT_MEMBER(BehaviorEvent, FormSubmit);
			KxEVENT_MEMBER(BehaviorEvent, FormReset);

			// Misc
			KxEVENT_MEMBER(BehaviorEvent, Animation);
			KxEVENT_MEMBER(BehaviorEvent, ActivateChild);

			KxEVENT_MEMBER(BehaviorEvent, VisualStatusChanged);
			KxEVENT_MEMBER(BehaviorEvent, DisabledStatusChanged);
			KxEVENT_MEMBER(BehaviorEvent, ContentChanged);
			KxEVENT_MEMBER(BehaviorEvent, UIStateChanged);

		protected:
			wxString m_EventName;
			ElementUID* m_SourceElement = nullptr;

		public:
			BehaviorEvent(Host& host)
				:Event(host)
			{
			}

		public:
			BehaviorEvent* Clone() const override
			{
				return new BehaviorEvent(*this);
			}
			
			wxString GetEventName() const
			{
				return m_EventName;
			}
			void SetEventName(const wxString& name)
			{
				m_EventName = name;
			}

			Element GetSourceElement() const
			{
				return RetrieveElement(m_SourceElement);
			}
			void SetSourceElement(const Element& element)
			{
				AcceptElement(m_SourceElement, element);
			}
	};
}
