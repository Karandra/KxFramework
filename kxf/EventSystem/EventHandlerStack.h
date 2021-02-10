#pragma once
#include "IEvtHandler.h"
#include "kxf/General/Enumerator.h"
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
				return EnumItems(Order::LastToFirst).CalcTotalCount();
			}
			Enumerator<IEvtHandler&> EnumItems(Order order, bool chainedItemsOnly = false) const
			{
				return [item = order == Order::FirstToLast ? m_Base : m_Top, this, order, chainedItemsOnly]() mutable -> optional_ref<IEvtHandler>
				{
					if (item && (!chainedItemsOnly || item != m_Base))
					{
						auto result = item;
						if (order == Order::FirstToLast)
						{
							item = item->GetPrevHandler();
						}
						else
						{
							item = item->GetNextHandler();
						}

						return *result;
					}
					return {};
				};
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
