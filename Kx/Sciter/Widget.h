#pragma once
#include "Kx/Sciter/Common.h"
#include "Kx/Sciter/Element.h"
#include "Kx/Sciter/EventDispatcher.h"
#include "Kx/EventSystem/EventHandlerStack.h"

namespace KxSciter
{
	class KX_API Widget: public wxEvtHandler
	{
		private:
			WidgetEventDispatcher m_EventDispatcher;
			KxEvtHandlerStack m_EventHandlerStack;
			Host& m_Host;

		private:
			void SetPreviousHandler(wxEvtHandler* evtHandler) override
			{
			}
			void SetNextHandler(wxEvtHandler* evtHandler) override
			{
			}

		public:
			Widget(Host& host)
				:m_EventDispatcher(host, *this), m_EventHandlerStack(*this), m_Host(host)
			{
			}
			Widget(const Widget&) = delete;
			virtual ~Widget() = default;

		public:
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

		public:
			Widget& operator=(const Widget&) = delete;
	};
}
