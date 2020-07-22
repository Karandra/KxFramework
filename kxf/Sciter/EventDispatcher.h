#pragma once
#include "Common.h"
#include "Node.h"
#include "Element.h"
#include "Events.h"
#include "kxf/wxWidgets/EvtHandlerWrapper.h"

namespace kxf::Sciter
{
	class Host;
	class Widget;
}

namespace kxf::Sciter
{
	class KX_API BasicEventDispatcher
	{
		friend class Host;

		private:
			static BOOL __stdcall CallHandleEvent(void* context, ElementHandle* element, uint32_t eventGroupID, void* parameters);
			static UINT __stdcall CallHandleNotification(void* notification, void* context);

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
			bool ProcessEvent(IEvent& event, const EventID& eventID);
			void QueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID);

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
			virtual EvtHandler& GetEvtHandler() = 0;
	};
}

namespace kxf::Sciter
{
	class KX_API EventDispatcher: public BasicEventDispatcher
	{
		private:
			EvtHandler& m_EvtHandler;

		public:
			EventDispatcher(Host& host, EvtHandler& evtHandler)
				:BasicEventDispatcher(host), m_EvtHandler(evtHandler)
			{
			}

		public:
			EvtHandler& GetEvtHandler() override
			{
				return m_EvtHandler;
			}
	};

	class KX_API WindowEventDispatcher: public BasicEventDispatcher
	{
		private:
			wxWindow& m_Window;
			wxWidgets::EvtHandlerWrapper m_Wrapper;

		public:
			WindowEventDispatcher(Host& host, wxWindow& window)
				:BasicEventDispatcher(host), m_Window(window), m_Wrapper(window)
			{
			}

		public:
			EvtHandler& GetEvtHandler() override
			{
				return m_Wrapper;
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
			EvtHandler& GetEvtHandler() override;
	};
}
