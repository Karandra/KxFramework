#pragma once
#include "Common.h"
#include "Node.h"
#include "Element.h"
#include "Events.h"

namespace KxFramework::Sciter
{
	class Host;
	class Widget;
}

namespace KxFramework::Sciter
{
	class KX_API BasicEventDispatcher
	{
		friend class Host;

		private:
			static BOOL CallHandleEvent(void* context, ElementHandle* element, uint32_t eventGroupID, void* parameters);
			static UINT CallHandleNotification(void* notification, void* context);

		private:
			Host& m_Host;

		private:
			int HandleLoadDataNotification(void* context);
			int HandleDataLoadedNotification(void* context);
			int HandlePostedNotification(void* context);
			bool HandleAttachBehaviorNotification(void* context);
			void HandleCriticalFailureNotification();
			void HandleDestroyedNotification();

			bool HandleInitializationEvent(ElementHandle* element, void* context);
			bool HandleKeyEvent(ElementHandle* element, void* context);
			bool HandleMouseEvent(ElementHandle* element, void* context);
			bool HandleFocusEvent(ElementHandle* element, void* context);
			bool HandleSizeEvent(ElementHandle* element, void* context);
			bool HandleTimerEvent(ElementHandle* element, void* context);
			bool HandleScrollEvent(ElementHandle* element, void* context);
			bool HandleDrawEvent(ElementHandle* element, void* context);
			bool HandleBehaviorEvent(ElementHandle* element, void* context);

			WXHWND GetSciterHandle() const;
			bool IsHostLevelDispatcher() const;
			bool ProcessEvent(wxEvent& event);
			void QueueEvent(std::unique_ptr<wxEvent> event);

			void AttachHost();
			void DetachHost();
			void AttachElement(Element& element);
			void DetachElement(Element& element);

		protected:
			bool SciterHandleEvent(ElementHandle* element, uint32_t eventGroupID, void* context);
			int SciterHandleNotification(void* context);

		public:
			BasicEventDispatcher(Host& host)
				:m_Host(host)
			{
			}
			virtual ~BasicEventDispatcher() = default;

		public:
			Host& GetHost() const
			{
				return m_Host;
			}
			virtual wxEvtHandler& GetEvtHandler() = 0;
	};
}

namespace KxFramework::Sciter
{
	class KX_API EventDispatcher: public BasicEventDispatcher
	{
		private:
			wxEvtHandler& m_EvtHandler;

		public:
			EventDispatcher(Host& host, wxEvtHandler& evtHandler)
				:BasicEventDispatcher(host), m_EvtHandler(evtHandler)
			{
			}

		public:
			wxEvtHandler& GetEvtHandler() override
			{
				return m_EvtHandler;
			}
	};

	class KX_API WindowEventDispatcher: public BasicEventDispatcher
	{
		private:
			wxWindow& m_Window;

		public:
			WindowEventDispatcher(Host& host, wxWindow& window)
				:BasicEventDispatcher(host), m_Window(window)
			{
			}

		public:
			wxEvtHandler& GetEvtHandler() override
			{
				return *m_Window.GetEventHandler();
			}
	};

	class KX_API WidgetEventDispatcher: public BasicEventDispatcher
	{
		private:
			Widget& m_Widget;

		public:
			WidgetEventDispatcher(Host& host, Widget& widget)
				:BasicEventDispatcher(host), m_Widget(widget)
			{
			}

		public:
			wxEvtHandler& GetEvtHandler() override;
	};
}
