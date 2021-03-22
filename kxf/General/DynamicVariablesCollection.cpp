#include "KxfPCH.h"
#include "DynamicVariablesCollection.h"

namespace kxf
{
	size_t DynamicVariablesCollection::DoClearItems(const String& ns)
	{
		size_t count = 0;
		if (ns.IsEmpty())
		{
			count = m_DynamicItems.size();
			m_DynamicItems.clear();
		}
		else
		{
			for (auto it = m_DynamicItems.begin(); it != m_DynamicItems.end(); ++it)
			{
				if (it->first.Namespace == ns)
				{
					count++;
					m_DynamicItems.erase(it);
				}
			}
		}
		return count + StaticVariablesCollection::DoClearItems(ns);
	}
	size_t DynamicVariablesCollection::DoGetItemCount(const String& ns) const
	{
		if (ns.IsEmpty())
		{
			return m_DynamicItems.size() + StaticVariablesCollection::DoGetItemCount(ns);
		}
		else
		{
			size_t count = 0;
			for (auto&& [descriptor, value]: m_DynamicItems)
			{
				if (descriptor.Namespace == ns)
				{
					count++;
				}
			}
			return count + StaticVariablesCollection::DoGetItemCount(ns);
		}
	}
	size_t DynamicVariablesCollection::DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const
	{
		size_t count = 0;
		bool canceled = false;

		for (auto&& [descriptor, value]: m_DynamicItems)
		{
			count++;
			if (!std::invoke(func, descriptor.Namespace, descriptor.Value, std::invoke(value, descriptor.Namespace, descriptor.Value)))
			{
				canceled = true;
				break;
			}
		}

		if (!canceled)
		{
			count += StaticVariablesCollection::DoEnumItems(std::move(func));
		}
		return count;
	}

	bool DynamicVariablesCollection::DoHasItem(const String& ns, const String& id) const
	{
		return m_DynamicItems.find({ns, id}) != m_DynamicItems.end() || StaticVariablesCollection::DoHasItem(ns, id);
	}
	Any DynamicVariablesCollection::DoGetItem(const String& ns, const String& id) const
	{
		auto it = m_DynamicItems.find({ns, id});
		if (it != m_DynamicItems.end())
		{
			return std::invoke(it->second, ns, id);
		}
		return StaticVariablesCollection::DoGetItem(ns, id);
	}
	void DynamicVariablesCollection::DoSetItem(const String& ns, const String& id, Any item)
	{
		if (auto func = std::move(item).QueryAs<TValue>())
		{
			m_DynamicItems.insert_or_assign({ns, id}, std::move(*func));
		}
		else
		{
			StaticVariablesCollection::DoSetItem(ns, id, std::move(item));
		}
	}
}
