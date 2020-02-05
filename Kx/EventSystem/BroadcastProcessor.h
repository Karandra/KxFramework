#pragma once
#include "EvtHandler.h"
#include "EventHandlerStack.h"
class KxBroadcastProcessor;
class KxBroadcastReciever;

namespace KxEventSystem
{
	class KX_API BroadcastProcessorHandler: public wxEvtHandler
	{
		private:
			KxBroadcastProcessor& m_Processor;

		protected:
			bool TryBefore(wxEvent& event) override;

		public:
			BroadcastProcessorHandler(KxBroadcastProcessor& processor);
	};

	class KX_API BroadcastRecieverHandler: public wxEvtHandler
	{
		friend class KxBroadcastReciever;

		protected:
			bool TryBefore(wxEvent& event) override;
			bool UnbindAll(std::optional<KxEventID> eventID = {});
	};
}

class KX_API KxBroadcastProcessor
{
	friend class KxBroadcastReciever;

	public:
		using Order = KxEvtHandlerStack::Order;

	private:
		KxEventSystem::BroadcastProcessorHandler m_EvtHandler;
		KxRefEvtHandler m_EvtHandlerWrapper;
		KxEvtHandlerStack m_Stack;
		Order m_Order = Order::LastToFirst;

	protected:
		virtual bool PreProcessEvent(wxEvent& event)
		{
			return true;
		}
		virtual void PostProcessEvent(wxEvent& event)
		{
		}

	public:
		KxBroadcastProcessor()
			:m_EvtHandlerWrapper(&m_EvtHandler), m_EvtHandler(*this), m_Stack(m_EvtHandler)
		{
		}
		virtual ~KxBroadcastProcessor() = default;

	public:
		bool AddReciever(KxBroadcastReciever& reciever);
		bool RemoveReciever(KxBroadcastReciever& reciever);

		bool HasRecieveres() const
		{
			return m_Stack.HasChainedItems();
		}
		size_t GetRecieveresCount() const
		{
			return m_Stack.GetCount();
		}

		template<class TFunc>
		wxEvtHandler* EnumRecieveres(Order order, TFunc&& func) const
		{
			return m_Stack.ForEachItem(order, std::forward<TFunc>(func), true);
		}

		template<class TFunc>
		wxEvtHandler* EnumRecieveres(TFunc&& func) const
		{
			return m_Stack.ForEachItem(m_Order, std::forward<TFunc>(func), true);
		}

		Order GetRecieversOrder() const
		{
			return m_Order;
		}
		void SetRecieversOrder(Order order)
		{
			m_Order = order;
		}

	public:
		bool ProcessEvent(wxEvent& event, std::optional<KxEventID> eventID = {})
		{
			return m_EvtHandlerWrapper.ProcessEvent(event, eventID);
		}
		template<class TEvent, class... Args> bool ProcessEvent(Args&&... arg)
		{
			return m_EvtHandlerWrapper.ProcessEvent(std::forward<Args>(arg)...);
		}
		template<class TEvent, class... Args> bool ProcessEvent(KxEventTag<TEvent> eventTag, Args&&... arg)
		{
			return m_EvtHandlerWrapper.ProcessEvent(eventTag, std::forward<Args>(arg)...);
		}
		template<class TEvent, class... Args> auto ProcessEventEx(KxEventTag<TEvent> eventTag, Args&&... arg)
		{
			return m_EvtHandlerWrapper.ProcessEventEx(eventTag, std::forward<Args>(arg)...);
		}

		void QueueEvent(std::unique_ptr<KxEvent> event, std::optional<KxEventID> eventID = {})
		{
			m_EvtHandlerWrapper.QueueEvent(std::move(event), eventID);
		}
		template<class TEvent, class... Args> void QueueEvent(Args&&... arg)
		{
			m_EvtHandlerWrapper.QueueEvent(std::forward<Args>(arg)...);
		}
		template<class TEvent, class... Args> void QueueEvent(KxEventTag<TEvent> eventTag, Args&&... arg)
		{
			m_EvtHandlerWrapper.QueueEvent(eventTag, std::forward<Args>(arg)...);
		}
		template<class TEvent, class... Args> auto QueueEventEx(KxEventTag<TEvent> eventTag, Args&&... arg)
		{
			return m_EvtHandlerWrapper.QueueEventEx(eventTag, std::forward<Args>(arg)...);
		}

		template<class TCallable, class... Args> void CallAfter(TCallable callable, Args&&... arg)
		{
			m_EvtHandlerWrapper.CallAfter(std::move(callable), std::forward<Args>(arg)...);
		}
};

class KX_API KxBroadcastReciever
{
	friend class KxBroadcastProcessor;

	private:
		KxEventSystem::BroadcastRecieverHandler m_EvtHandler;
		KxRefEvtHandler m_EvtHandlerWrapper;
		KxBroadcastProcessor& m_Processor;

	private:
		bool PreProcessEvent(wxEvent& event);
		void PostProcessEvent(wxEvent& event);
		void FinalPostProcessEvent(wxEvent& event);

	protected:
		wxEvtHandler& GetEvtHandler()
		{
			return m_EvtHandler;
		}

	public:
		KxBroadcastReciever(KxBroadcastProcessor& processor)
			:m_EvtHandlerWrapper(&m_EvtHandler), m_Processor(processor)
		{
			m_Processor.AddReciever(*this);
		}
		virtual ~KxBroadcastReciever()
		{
			m_Processor.RemoveReciever(*this);
		}

	public:
		KxBroadcastProcessor& GetProcessor()
		{
			return m_Processor;
		}
		const KxBroadcastProcessor& GetProcessor() const
		{
			return m_Processor;
		}

	public:
		// Bind free or static function
		template<class TEvent, class TEventArg>
		bool Bind(KxEventTag<TEvent> eventTag, void(*func)(TEventArg&))
		{
			return m_EvtHandlerWrapper.Bind(eventTag, [this, func](TEvent& event)
			{
				if (PreProcessEvent(event))
				{
					func(event);
					PostProcessEvent(event);
				}
				FinalPostProcessEvent(event);
			});
		}

		// Bind functor or lambda function
		template<class TEvent, class TFunctor>
		bool Bind(KxEventTag<TEvent> eventTag, const TFunctor& func)
		{
			return m_EvtHandlerWrapper.Bind(eventTag, [this, func](TEvent& event)
			{
				if (PreProcessEvent(event))
				{
					func(event);
					PostProcessEvent(event);
				}
				FinalPostProcessEvent(event);
			});
		}

		// Bind member function
		template<class TEvent, class TClass, class TEventArg, class TEventHandler>
		bool Bind(KxEventTag<TEvent> eventTag, void(TClass::* method)(TEventArg&), TEventHandler* handler)
		{
			return m_EvtHandlerWrapper.Bind(eventTag, [this, method, handler](TEvent& event)
			{
				if (PreProcessEvent(event))
				{
					std::invoke(method, handler, event);
					PostProcessEvent(event);
				}
				FinalPostProcessEvent(event);
			});
		}

		// Bind member function
		template<class TEvent, class TClass, class TEventArg>
		bool Bind(KxEventTag<TEvent> eventTag, void(TClass::* method)(TEventArg&))
		{
			static_assert(std::is_base_of_v<KxBroadcastReciever, TClass>, "TClass must be derived from KxBroadcastReciever");

			return m_EvtHandlerWrapper.Bind(eventTag, [this, method](TEvent& event)
			{
				if (PreProcessEvent(event))
				{
					std::invoke(method, static_cast<TClass*>(this), event);
					PostProcessEvent(event);
				}
				FinalPostProcessEvent(event);
			});
		}

		// Unbind all functions bound to this event
		template<class TEvent> bool UnbindAll(KxEventTag<TEvent> eventTag)
		{
			return m_EvtHandler.UnbindAll(eventTag);
		}

		// Unbind all events
		bool UnbindAll()
		{
			return m_EvtHandler.UnbindAll();
		}
};
