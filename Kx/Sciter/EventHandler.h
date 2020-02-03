#pragma once
#include "Common.h"
#include "Node.h"
#include "Element.h"
#include "Events.h"

namespace KxSciter
{
	class Host;
}

namespace KxSciter
{
	class KX_API EventHandler
	{
		friend class Host;

		private:
			static BOOL CallHostEventhandler(void* context, ElementHandle* element, uint32_t eventGroupID, void* parameters);

		private:
			Host& m_Host;

		private:
			int HandleLoadDataNotification(void* context);
			int HandleDataLoadedNotification(void* context);
			int HandleAttachBehaviorNotification(void* context);
			int HandlePostedNotification(void* context);
			int handleCriticalFailureNotification();
			int HandleDestroyedNotification();

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

			void AttachHost();
			void DetachHost();
			void AttachElement(Element& element);
			void DetachElement(Element& element);

		public:
			EventHandler(Host& host)
				:m_Host(host)
			{
			}
			virtual ~EventHandler() = default;

		public:
			bool SciterHandleEvent(ElementHandle* element, uint32_t eventGroupID, void* context);
			int SciterHandleNotify(void* context);

			Host& GetHost() const
			{
				return m_Host;
			}
			bool ProcessEvent(wxEvent& event);
			virtual wxEvtHandler& GetEvtHandler() = 0;
	};
}

namespace KxSciter
{
	class KX_API RegularEventHandler: public EventHandler
	{
		private:
			wxEvtHandler& m_EvtHandler;

		public:
			RegularEventHandler(Host& host, wxEvtHandler& window)
				:EventHandler(host), m_EvtHandler(window)
			{
			}

		public:
			wxEvtHandler& GetEvtHandler() override
			{
				return m_EvtHandler;
			}
	};

	class KX_API WindowEventHandler: public EventHandler
	{
		private:
			wxWindow& m_Window;

		public:
			WindowEventHandler(Host& host, wxWindow& window)
				:EventHandler(host), m_Window(window)
			{
			}

		public:
			wxEvtHandler& GetEvtHandler() override
			{
				return *m_Window.GetEventHandler();
			}
	};
}