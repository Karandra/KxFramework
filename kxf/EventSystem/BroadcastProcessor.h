#pragma once
#include "EvtHandler.h"
#include "EventHandlerStack.h"
#include "EvtHandlerDelegate.h"

namespace kxf::EventSystem
{
	class BroadcastProcessor;
	class BroadcastReciever;
}

namespace kxf::EventSystem
{
	class KX_API BroadcastProcessorHandler final: public EvtHandler
	{
		private:
			BroadcastProcessor& m_Processor;

		protected:
			bool TryBefore(IEvent& event) override;

		public:
			BroadcastProcessorHandler(BroadcastProcessor& processor)
				:m_Processor(processor)
			{
			}
	};

	class KX_API BroadcastRecieverHandler final: public EvtHandler
	{
		friend class BroadcastReciever;

		protected:
			bool TryBefore(IEvent& event) override;
	};
}

namespace kxf::EventSystem
{
	class KX_API BroadcastProcessor: public EvtHandlerDelegate
	{
		friend class BroadcastReciever;

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
			BroadcastProcessor()
				:EvtHandlerDelegate(m_EvtHandler), m_EvtHandler(*this), m_Stack(m_EvtHandler)
			{
			}
			virtual ~BroadcastProcessor() = default;

		public:
			bool AddReciever(BroadcastReciever& reciever);
			bool RemoveReciever(BroadcastReciever& reciever);

			bool HasRecieveres() const
			{
				return m_Stack.HasChainedItems();
			}
			size_t GetRecieveresCount() const
			{
				return m_Stack.GetCount();
			}

			template<class TFunc>
			EvtHandler* EnumRecieveres(Order order, TFunc&& func) const
			{
				return m_Stack.ForEachItem(order, std::forward<TFunc>(func), true);
			}

			template<class TFunc>
			EvtHandler* EnumRecieveres(TFunc&& func) const
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

namespace kxf::EventSystem
{
	class KX_API BroadcastReciever: public EvtHandlerDelegate
	{
		friend class BroadcastProcessor;

		private:
			EventSystem::BroadcastRecieverHandler m_EvtHandler;
			BroadcastProcessor& m_Processor;

		private:
			bool PreProcessEvent(IEvent& event);
			void PostProcessEvent(IEvent& event);
			void FinalPostProcessEvent(IEvent& event);

			void StopPropagation(IEvent& event)
			{
				IWidgetEvent* commandEvent = nullptr;
				if (event.QueryInterface(commandEvent))
				{
					commandEvent->StopPropagation();
				}
			}

		protected:
			EvtHandler& GetEvtHandler()
			{
				return m_EvtHandler;
			}

		public:
			BroadcastReciever(BroadcastProcessor& processor)
				:EvtHandlerDelegate(m_EvtHandler), m_Processor(processor)
			{
				m_Processor.AddReciever(*this);
			}
			virtual ~BroadcastReciever()
			{
				m_Processor.RemoveReciever(*this);
			}

		public:
			BroadcastProcessor& GetProcessor()
			{
				return m_Processor;
			}
			const BroadcastProcessor& GetProcessor() const
			{
				return m_Processor;
			}
	};
}
