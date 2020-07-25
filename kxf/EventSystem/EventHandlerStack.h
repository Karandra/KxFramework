#pragma once
#include "IEvtHandler.h"
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
			IEvtHandler* m_Base = nullptr;
			IEvtHandler* m_Top = nullptr;

		public:
			EvtHandlerStack(IEvtHandler& first)
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
			bool Push(IEvtHandler& evtHandler);
			bool Remove(IEvtHandler& evtHandler);
			IEvtHandler* Pop();

			IEvtHandler* GetBase() const
			{
				return m_Base;
			}
			IEvtHandler* GetTop() const
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
				ForEachItem(Order::LastToFirst, [&count](IEvtHandler& chainItem)
				{
					count++;
					return true;
				});
				return count;
			}

			template<class TFunc>
			IEvtHandler* ForEachItem(Order order, TFunc&& func, bool chainedItemsOnly = false) const
			{
				auto TestItem = [&](IEvtHandler* item)
				{
					return item && (!chainedItemsOnly || item != m_Base);
				};

				switch (order)
				{
					case Order::FirstToLast:
					{
						for (IEvtHandler* item = m_Base; TestItem(item); item = item->GetPrevHandler())
						{
							if (!func(*item))
							{
								return item;
							}
						}
					}
					case Order::LastToFirst:
					{
						for (IEvtHandler* item = m_Top; TestItem(item); item = item->GetNextHandler())
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
