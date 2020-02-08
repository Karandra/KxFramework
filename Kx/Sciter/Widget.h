#pragma once
#include "Kx/Sciter/Common.h"
#include "Kx/Sciter/Element.h"
#include "Kx/Sciter/EventDispatcher.h"
#include "Kx/EventSystem/EventHandlerStack.h"

namespace KxSciter
{
	class WidgetFactory;

	enum class LayoutFlow
	{
		None,
		Horizontal,
		Vertical
	};
}

namespace KxSciter
{
	class KX_API Widget: public wxEvtHandler
	{
		friend class BasicEventDispatcher;

		private:
			WidgetEventDispatcher m_EventDispatcher;
			KxEvtHandlerStack m_EventHandlerStack;
			Element m_Element;
			WidgetFactory& m_Factory;
			Host& m_Host;

		private:
			void SetPreviousHandler(wxEvtHandler* evtHandler) override
			{
			}
			void SetNextHandler(wxEvtHandler* evtHandler) override
			{
			}

		protected:
			using wxEvtHandler::ProcessEvent;
			using wxEvtHandler::ProcessEventLocally;
			using wxEvtHandler::ProcessThreadEvent;
			using wxEvtHandler::SafelyProcessEvent;
			using wxEvtHandler::ProcessPendingEvents;
			using wxEvtHandler::AddPendingEvent;
			using wxEvtHandler::QueueEvent;

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
			wxEvtHandler& GetEventHandler()
			{
				return *m_EventHandlerStack.GetTop();
			}

			void PushEventHandler(wxEvtHandler& evtHandler)
			{
				m_EventHandlerStack.Push(evtHandler);
			}
			wxEvtHandler* PopEventHandler()
			{
				return m_EventHandlerStack.Pop();
			}
			bool RemoveEventHandler(wxEvtHandler& evtHandler)
			{
				return m_EventHandlerStack.Remove(evtHandler);
			}

			bool ProcessWidgetEvent(wxEvent& event)
			{
				return GetEventHandler().ProcessEvent(event);
			}
			bool ProcessWidgetEventLocally(wxEvent& event)
			{
				return GetEventHandler().ProcessEventLocally(event);
			}
			bool HandleWidgetEvent(wxEvent& event)
			{
				// SafelyProcessEvent() will handle exceptions nicely
				return GetEventHandler().SafelyProcessEvent(event);
			}

			// Layout
			LayoutFlow GetLayoutFlow() const;
			void SetLayoutFlow(LayoutFlow flow);

			// Size and position
			wxRect GetRect() const
			{
				return m_Element.GetRect();
			}
			wxSize GetSize() const
			{
				return GetRect().GetSize();
			}
			void SetSize(const wxSize& size);

			wxSize GetMinSize() const
			{
				return m_Element.GetMinSize();
			}
			wxSize GetMaxSize() const
			{
				return m_Element.GetMaxSize();
			}

			wxPoint GetPosition() const
			{
				return GetRect().GetPosition();
			}
			void SetPosition(const wxPoint& pos);
			
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
