#include "KxfPCH.h"
#include "CombinedVariablesCollection.h"

namespace kxf
{
	size_t CombinedVariablesCollection::DoClearItems(const String& ns)
	{
		return m_EditableCollection->ClearItems(ns);
	}
	size_t CombinedVariablesCollection::DoGetItemCount(const String& ns) const
	{
		size_t count = 0;
		for (IVariablesCollection* collection: m_Items)
		{
			count += collection->GetItemCount(ns);
		}
		return count;
	}
	size_t CombinedVariablesCollection::DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const
	{
		size_t count = 0;
		bool canContinue = true;

		for (IVariablesCollection* collection: m_Items)
		{
			count += collection->EnumItems([&](const String& ns, const String& id, Any value)
			{
				canContinue = std::invoke(func, ns, id, std::move(value));
				return canContinue;
			});

			if (!canContinue)
			{
				break;
			}
		}
		return count;
	}

	bool CombinedVariablesCollection::DoHasItem(const String& ns, const String& id) const
	{
		for (IVariablesCollection* collection: m_Items)
		{
			if (collection->HasItem(ns, id))
			{
				return true;
			}
		}
		return false;
	}
	Any CombinedVariablesCollection::DoGetItem(const String& ns, const String& id) const
	{
		for (IVariablesCollection* collection: m_Items)
		{
			if (auto value = collection->GetItem(ns, id))
			{
				return value;
			}
		}
		return {};
	}

	void CombinedVariablesCollection::DoSetItem(const String& ns, const String& id, Any item)
	{
		m_EditableCollection->SetItem(ns, id, std::move(item));
	}
}
