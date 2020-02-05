#pragma once
#include "EvtHandler.h"

class KX_API KxEvtHandlerStack
{
	public:
		enum class Order
		{
			LastToFirst,
			FirstToLast,
		};

	private:
		wxEvtHandler* m_First = nullptr;
		wxEvtHandler* m_Last = nullptr;

	public:
		KxEvtHandlerStack(wxEvtHandler& first)
			:m_First(&first), m_Last(&first)
		{
		}

	public:
		wxEvtHandler* GetBase() const
		{
			return m_First;
		}
		wxEvtHandler* GetTop() const
		{
			return m_Last;
		}
		size_t GetCount() const
		{
			size_t count = 0;
			ForEachItem<Order::LastToFirst>([&count](wxEvtHandler& chainItem)
			{
				count++;
				return true;
			});
			return count;
		}
		bool HasChainedItems() const
		{
			return m_First != m_Last;
		}

		bool Push(wxEvtHandler& evtHandler);
		bool Remove(wxEvtHandler& evtHandler);
		wxEvtHandler* Pop();

	public:
		template<class TFunc>
		wxEvtHandler* ForEachItem(Order order, TFunc&& func, bool chainedItemsOnly = false) const
		{
			auto TestItem = [&](wxEvtHandler* item)
			{
				return item && (!chainedItemsOnly || item != m_First);
			};

			switch (order)
			{
				case Order::FirstToLast:
				{
					for (wxEvtHandler* item = m_First; TestItem(); item = item->GetPreviousHandler())
					{
						if (!func(*item))
						{
							return item;
						}
					}
				}
				case Order::LastToFirst:
				{
					for (wxEvtHandler* item = m_Last; TestItem(); item = item->GetNextHandler())
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
};
