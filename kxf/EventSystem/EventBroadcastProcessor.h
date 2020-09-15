#pragma once
#include "IEvtHandler.h"
#include "EvtHandler.h"
#include "EventHandlerStack.h"
#include "EvtHandlerDelegate.h"

namespace kxf
{
	class EventBroadcastProcessor;
	class EventBroadcastReciever;
}

namespace kxf::EventSystem
{
	class KX_API BroadcastProcessorHandler final: public EvtHandler
	{
		private:
			EventBroadcastProcessor& m_Processor;

		protected:
			bool TryBefore(IEvent& event) override;

		public:
			BroadcastProcessorHandler(EventBroadcastProcessor& processor)
				:m_Processor(processor)
			{
			}
	};

	class KX_API BroadcastRecieverHandler final: public EvtHandler
	{
		friend class EventBroadcastReciever;

		protected:
			bool TryBefore(IEvent& event) override;
	};
}

namespace kxf
{
	class KX_API EventBroadcastProcessor: public EvtHandlerDelegate
	{
		friend class EventBroadcastReciever;

		public:
			using Order = EvtHandlerStack::Order;

		private:
			EventSystem::BroadcastProcessorHandler m_EvtHandler;
			EvtHandlerStack m_Stack;
			Order m_Order = Order::LastToFirst;

		protected:
			virtual bool PreProcessEvent(IEvent& event)
			{
				return true;
			}
			virtual void PostProcessEvent(IEvent& event)
			{
			}

		public:
			EventBroadcastProcessor()
				:EvtHandlerDelegate(m_EvtHandler), m_EvtHandler(*this), m_Stack(m_EvtHandler)
			{
			}
			virtual ~EventBroadcastProcessor() = default;

		public:
			bool AddReciever(EventBroadcastReciever& reciever);
			bool RemoveReciever(EventBroadcastReciever& reciever);

			bool HasRecieveres() const
			{
				return m_Stack.HasChainedItems();
			}
			size_t GetRecieveresCount() const
			{
				return m_Stack.GetCount();
			}

			template<class TFunc>
			IEvtHandler* EnumRecieveres(Order order, TFunc&& func) const
			{
				return m_Stack.ForEachItem(order, std::forward<TFunc>(func), true);
			}

			template<class TFunc>
			IEvtHandler* EnumRecieveres(TFunc&& func) const
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
	};
}

namespace kxf
{
	class KX_API EventBroadcastReciever: public EvtHandlerDelegate
	{
		friend class EventBroadcastProcessor;

		private:
			EventSystem::BroadcastRecieverHandler m_EvtHandler;
			EventBroadcastProcessor& m_Processor;

		private:
			bool PreProcessEvent(IEvent& event);
			void PostProcessEvent(IEvent& event);
			void FinalPostProcessEvent(IEvent& event);

			void StopPropagation(IEvent& event)
			{
				object_ptr<IWidgetEvent> commandEvent;
				if (event.QueryInterface(commandEvent))
				{
					commandEvent->StopPropagation();
				}
			}

		protected:
			IEvtHandler& GetEvtHandler()
			{
				return m_EvtHandler;
			}

		public:
			EventBroadcastReciever(EventBroadcastProcessor& processor)
				:EvtHandlerDelegate(m_EvtHandler), m_Processor(processor)
			{
				m_Processor.AddReciever(*this);
			}
			virtual ~EventBroadcastReciever()
			{
				m_Processor.RemoveReciever(*this);
			}

		public:
			EventBroadcastProcessor& GetProcessor()
			{
				return m_Processor;
			}
			const EventBroadcastProcessor& GetProcessor() const
			{
				return m_Processor;
			}
	};
}
