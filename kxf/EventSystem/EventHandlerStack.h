#pragma once
#include "EvtHandler.h"
#include "kxf/Utility/Common.h"

namespace kxf
{
	class KX_API EvtHandlerStack
	{
		public:
			enum class Order
			{
				LastToFirst,
				FirstToLast,
			};

		private:
			EvtHandler* m_Base = nullptr;
			EvtHandler* m_Top = nullptr;

		public:
			EvtHandlerStack(EvtHandler& first)
				:m_Base(&first), m_Top(&first)
			{
			}
			EvtHandlerStack(EvtHandlerStack&& other) noexcept
			{
				*this = std::move(other);
			}
			EvtHandlerStack(const EvtHandlerStack&) = delete;
			virtual ~EvtHandlerStack() = default;

		public:
			bool Push(EvtHandler& evtHandler);
			bool Remove(EvtHandler& evtHandler);
			EvtHandler* Pop();

			EvtHandler* GetBase() const
			{
				return m_Base;
			}
			EvtHandler* GetTop() const
			{
				return m_Top;
			}
			bool HasChainedItems() const
			{
				return m_Base != m_Top;
			}
			size_t GetCount() const
			{
				size_t count = 0;
				ForEachItem(Order::LastToFirst, [&count](EvtHandler& chainItem)
				{
					count++;
					return true;
				});
				return count;
			}

			template<class TFunc>
			EvtHandler* ForEachItem(Order order, TFunc&& func, bool chainedItemsOnly = false) const
			{
				auto TestItem = [&](EvtHandler* item)
				{
					return item && (!chainedItemsOnly || item != m_Base);
				};

				switch (order)
				{
					case Order::FirstToLast:
					{
						for (EvtHandler* item = m_Base; TestItem(item); item = item->GetPreviousHandler())
						{
							if (!func(*item))
							{
								return item;
							}
						}
					}
					case Order::LastToFirst:
					{
						for (EvtHandler* item = m_Top; TestItem(item); item = item->GetNextHandler())
						{
							if (!func(*item))
							{
								return item;
							}
						}
					}
				};
				return nullptr;
			}

		public:
			EvtHandlerStack& operator=(EvtHandlerStack&& other) noexcept
			{
				m_Base = Utility::ExchangeResetAndReturn(other.m_Base, nullptr);
				m_Top = Utility::ExchangeResetAndReturn(other.m_Top, nullptr);

				return *this;
			}
			EvtHandlerStack& operator=(const EvtHandlerStack&) = delete;
	};
}
