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
	class KX_API BasicEventHandler
	{
		friend class Host;

		private:
			static BOOL CallHostEventHandler(void* context, ElementHandle* element, uint32_t eventGroupID, void* parameters);

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
			bool IsHostLevelHandler() const;
			bool ProcessEvent(wxEvent& event);
			void QueueEvent(std::unique_ptr<wxEvent> event);

			void AttachHost();
			void DetachHost();
			void AttachElement(Element& element);
			void DetachElement(Element& element);

		protected:
			bool SciterHandleEvent(ElementHandle* element, uint32_t eventGroupID, void* context);
			int SciterHandleNotify(void* context);

		public:
			BasicEventHandler(Host& host)
				:m_Host(host)
			{
			}
			virtual ~BasicEventHandler() = default;

		public:
			Host& GetHost() const
			{
				return m_Host;
			}
			virtual wxEvtHandler& GetEvtHandler() = 0;
	};
}

namespace KxSciter
{
	class KX_API EventHandler: public BasicEventHandler
	{
		private:
			wxEvtHandler& m_EvtHandler;

		public:
			EventHandler(Host& host, wxEvtHandler& window)
				:BasicEventHandler(host), m_EvtHandler(window)
			{
			}

		public:
			wxEvtHandler& GetEvtHandler() override
			{
				return m_EvtHandler;
			}
	};

	class KX_API WindowEventHandler: public BasicEventHandler
	{
		private:
			wxWindow& m_Window;

		public:
			WindowEventHandler(Host& host, wxWindow& window)
				:BasicEventHandler(host), m_Window(window)
			{
			}

		public:
			wxEvtHandler& GetEvtHandler() override
			{
				return *m_Window.GetEventHandler();
			}
	};
}
