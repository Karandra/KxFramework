#pragma once
#include "kxf/Sciter/Common.h"
#include "kxf/Sciter/Element.h"
#include "kxf/Sciter/EventDispatcher.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/EventSystem/EvtHandlerAccessor.h"
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
	class KX_API Widget: public IEvtHandler
	{
		friend class BasicEventDispatcher;

		private:
			WidgetEventDispatcher m_EventDispatcher;
			EvtHandlerStack m_EventHandlerStack;
			EvtHandler m_EvtHandler;

			Element m_Element;
			WidgetFactory& m_Factory;
			Host& m_Host;

		protected:
			auto AccessThisEvtHandler()
			{
				return EventSystem::EvtHandlerAccessor(m_EvtHandler);
			}
			auto AccessTopEvtHandler()
			{
				return EventSystem::EvtHandlerAccessor(GetEventHandler());
			}

			// IEvtHandler
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<EventFlag> flags = {}) override
			{
				return AccessThisEvtHandler().DoBind(eventID, std::move(executor), flags);
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor) override
			{
				return AccessThisEvtHandler().DoUnbind(eventID, executor);
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot) override
			{
				return AccessThisEvtHandler().DoUnbind(bindSlot);
			}

			bool OnDynamicBind(EventItem& eventItem) override
			{
				return AccessThisEvtHandler().OnDynamicBind(eventItem);
			}
			bool OnDynamicUnbind(EventItem& eventItem) override
			{
				return AccessThisEvtHandler().OnDynamicUnbind(eventItem);
			}

			void DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, UniversallyUniqueID uuid = {}) override
			{
				return AccessTopEvtHandler().DoQueueEvent(std::move(event), eventID, std::move(uuid));
			}
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, IEvtHandler* onlyIn = nullptr) override
			{
				return AccessTopEvtHandler().DoProcessEvent(event, eventID, onlyIn);
			}
			bool DoProcessEventSafely(IEvent& event, const EventID& eventID = {}) override
			{
				return AccessTopEvtHandler().DoProcessEventSafely(event, eventID);
			}
			bool DoProcessEventLocally(IEvent& event, const EventID& eventID = {}) override
			{
				return AccessTopEvtHandler().DoProcessEventLocally(event, eventID);
			}

			bool TryBefore(IEvent& event) override
			{
				return AccessThisEvtHandler().TryBefore(event);
			}
			bool TryAfter(IEvent& event) override
			{
				return AccessThisEvtHandler().TryAfter(event);
			}

		public:
			Widget(Host& host, WidgetFactory& factory, const Element& element)
				:m_EventDispatcher(host, *this), m_EventHandlerStack(m_EvtHandler), m_Host(host), m_Factory(factory), m_Element(element)
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
			IEvtHandler& GetEventHandler()
			{
				return *m_EventHandlerStack.GetTop();
			}

			void PushEventHandler(IEvtHandler& evtHandler)
			{
				m_EventHandlerStack.Push(evtHandler);
			}
			IEvtHandler* PopEventHandler()
			{
				return m_EventHandlerStack.Pop();
			}
			bool RemoveEventHandler(IEvtHandler& evtHandler)
			{
				if (&evtHandler != &m_EvtHandler)
				{
					return m_EventHandlerStack.Remove(evtHandler);
				}
				return false;
			}

			// IEvtHandler
			bool ProcessPendingEvents() override
			{
				return m_EvtHandler.ProcessPendingEvents();
			}
			size_t DiscardPendingEvents() override
			{
				return m_EvtHandler.DiscardPendingEvents();
			}

			IEvtHandler* GetPrevHandler() const override
			{
				return m_EvtHandler.GetPrevHandler();
			}
			IEvtHandler* GetNextHandler() const override
			{
				return m_EvtHandler.GetNextHandler();
			}
			void SetPrevHandler(IEvtHandler* evtHandler) override
			{
				// Can't chain widgets
			}
			void SetNextHandler(IEvtHandler* evtHandler) override
			{
				// Can't chain widgets
			}

			void Unlink() override
			{
				m_EvtHandler.Unlink();
			}
			bool IsUnlinked() const override
			{
				return m_EvtHandler.IsUnlinked();
			}

			bool IsEventProcessingEnabled() const override
			{
				return m_EvtHandler.IsEventProcessingEnabled();
			}
			void EnableEventProcessing(bool enable = true) override
			{
				m_EvtHandler.EnableEventProcessing(enable);
			}

		public:
			Widget& operator=(const Widget&) = delete;
	};
}
