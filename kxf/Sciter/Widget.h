#pragma once
#include "kxf/Sciter/Common.h"
#include "kxf/Sciter/Element.h"
#include "kxf/Sciter/EventDispatcher.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/EventSystem/EventHandlerStack.h"

namespace kxf::Sciter
{
	class WidgetFactory;

	enum class LayoutFlow
	{
		None,
		Horizontal,
		Vertical
	};
}

namespace kxf::Sciter
{
	class KX_API Widget: public EvtHandler
	{
		friend class BasicEventDispatcher;

		private:
			WidgetEventDispatcher m_EventDispatcher;
			EvtHandlerStack m_EventHandlerStack;
			Element m_Element;
			WidgetFactory& m_Factory;
			Host& m_Host;

		private:
			void SetPrevHandler(EvtHandler* evtHandler) override
			{
			}
			void SetNextHandler(EvtHandler* evtHandler) override
			{
			}

		public:
			Widget(Host& host, WidgetFactory& factory, const Element& element)
				:m_EventDispatcher(host, *this), m_EventHandlerStack(*this), m_Host(host), m_Factory(factory), m_Element(element)
			{
			}
			Widget(const Widget&) = delete;
			virtual ~Widget() = default;

		public:
			// General
			Element& GetElement()
			{
				return m_Element;
			}
			const Element& GetElement() const
			{
				return m_Element;
			}
			Host& GetHost() const
			{
				return m_Host;
			}

			// Factory
			WidgetFactory& GetFactory() const
			{
				return m_Factory;
			}

			// Event handler chain
			EvtHandler& GetEventHandler()
			{
				return *m_EventHandlerStack.GetTop();
			}

			void PushEventHandler(EvtHandler& evtHandler)
			{
				m_EventHandlerStack.Push(evtHandler);
			}
			EvtHandler* PopEventHandler()
			{
				return m_EventHandlerStack.Pop();
			}
			bool RemoveEventHandler(EvtHandler& evtHandler)
			{
				return m_EventHandlerStack.Remove(evtHandler);
			}

			bool ProcessWidgetEvent(IEvent& event, const EventID& eventID = {})
			{
				return GetEventHandler().ProcessEvent(event, eventID);
			}
			bool ProcessWidgetEventLocally(IEvent& event, const EventID& eventID = {})
			{
				return GetEventHandler().ProcessEventLocally(event, eventID);
			}
			bool HandleWidgetEvent(IEvent& event, const EventID& eventID = {})
			{
				// SafelyProcessEvent() will handle exceptions nicely
				return GetEventHandler().ProcessEventSafely(event, eventID);
			}

			// Layout
			LayoutFlow GetLayoutFlow() const;
			void SetLayoutFlow(LayoutFlow flow);

			// Size and position
			Rect GetRect() const
			{
				return m_Element.GetRect();
			}
			Size GetSize() const
			{
				return GetRect().GetSize();
			}
			void SetSize(const Size& size);

			Size GetMinSize() const
			{
				return m_Element.GetMinSize();
			}
			Size GetMaxSize() const
			{
				return m_Element.GetMaxSize();
			}

			Point GetPosition() const
			{
				return GetRect().GetPosition();
			}
			void SetPosition(const Point& pos);
			
			// Focus
			bool IsFocusable() const
			{
				return m_Element.IsFocusable();
			}
			bool HasFocus() const
			{
				return m_Element.HasFocus();
			}
			void SetFocus()
			{
				return m_Element.SetFocus();
			}

		public:
			Widget& operator=(const Widget&) = delete;
	};
}
