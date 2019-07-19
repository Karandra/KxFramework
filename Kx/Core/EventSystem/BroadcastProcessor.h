#pragma once
#include "EvtHandler.h"
class KxBroadcastProcessor;
class KxBroadcastReciever;

namespace Kx::EventSystem
{
	class KX_API BroadcastProcessorHandler: public wxEvtHandler
	{
		private:
			KxBroadcastProcessor& m_Processor;

		protected:
			bool TryBefore(wxEvent& event) override;

		public:
			BroadcastProcessorHandler(KxBroadcastProcessor& processor)
				:m_Processor(processor)
			{
			}
	};

	class KX_API BroadcastRecieverHandler: public wxEvtHandler
	{
		friend class KxBroadcastReciever;

		protected:
			bool TryBefore(wxEvent& event) override;
			bool UnbindAll(KxEventID eventID);
	};
}

class KX_API KxBroadcastProcessor
{
	friend class KxBroadcastReciever;

	public:
		enum class Order
		{
			LastToFirst,
			FirstToLast,
		};

	private:
		Kx::EventSystem::BroadcastProcessorHandler m_EvtHandler;
		KxRefEvtHandler m_EvtHandlerWrapper;
		wxEvtHandler* m_LastEvtHandler = nullptr;
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
			:m_EvtHandlerWrapper(&m_EvtHandler), m_EvtHandler(*this), m_LastEvtHandler(&m_EvtHandler)
		{
		}
		virtual ~KxBroadcastProcessor() = default;

	public:
		bool AddReciever(KxBroadcastReciever& reciever);
		bool RemoveReciever(KxBroadcastReciever& reciever);

		bool HasRecieveres() const
		{
			return m_EvtHandler.GetNextHandler() != nullptr;
		}
		size_t GetRecieveresCount() const
		{
			size_t count = 0;
			EnumRecieveres(m_Order, [&count](wxEvtHandler& item)
			{
				count++;
				return true;
			});
			return count;
		}
		
		template<Order order, class TFunc> wxEvtHandler* EnumRecieveres(TFunc&& func) const
		{
			if constexpr(order == Order::FirstToLast)
			{
				for (wxEvtHandler* item = m_EvtHandler.GetNextHandler(); item; item = item->GetNextHandler())
				{
					if (!func(*item))
					{
						return item;
					}
				}
			}
			else if constexpr(order == Order::LastToFirst)
			{
				for (wxEvtHandler* item = m_LastEvtHandler; item != &m_EvtHandler; item = item->GetPreviousHandler())
				{
					if (!func(*item))
					{
						return item;
					}
				}
			}
			else
			{
				static_assert(false, "invalid order");
			}
			return nullptr;
		}
		template<class TFunc> wxEvtHandler* EnumRecieveres(Order order, TFunc&& func) const
		{
			switch (order)
			{
				case Order::FirstToLast:
				{
					return EnumRecieveres<Order::FirstToLast>(std::forward<TFunc>(func));
				}
				case Order::LastToFirst:
				{
					return EnumRecieveres<Order::LastToFirst>(std::forward<TFunc>(func));
				}
			};
			return nullptr;
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
		Kx::EventSystem::BroadcastRecieverHandler m_EvtHandler;
		KxRefEvtHandler m_EvtHandlerWrapper;
		KxBroadcastProcessor& m_Processor;

	private:
		bool PreProcessEvent(wxEvent& event)
		{
			return m_Processor.PreProcessEvent(event);
		}
		void PostProcessEvent(wxEvent& event)
		{
			m_Processor.PostProcessEvent(event);
		}
		void FinalPostProcessEvent(wxEvent& event)
		{
			event.Skip();
		}

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
		template<class TEvent>
		bool UnbindAll(KxEventTag<TEvent> eventTag)
		{
			return m_EvtHandler.UnbindAll(eventTag);
		}
};
