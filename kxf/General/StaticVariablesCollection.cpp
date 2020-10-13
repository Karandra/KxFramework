#include "stdafx.h"
#include "StaticVariablesCollection.h"

namespace kxf
{
	size_t StaticVariablesCollection::DoGetItemCount(const String& ns) const
	{
		if (ns.IsEmpty())
		{
			return m_Items.size();
		}
		else
		{
			size_t count = 0;
			for (auto&& [descriptor, value]: m_Items)
			{
				if (descriptor.first == ns)
				{
					count++;
				}
			}
			return count;
		}
	}
	size_t StaticVariablesCollection::DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const
	{
		size_t count = 0;
		for (auto&& [descriptor, value]: m_Items)
		{
			count++;
			if (!std::invoke(func, descriptor.first, descriptor.second, value))
			{
				break;
			}
		}
		return count;
	}
}
