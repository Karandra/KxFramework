#pragma once
#include "Common.h"
#include "IVariablesCollection.h"
#include <map>

namespace kxf
{
	class KX_API StaticVariablesCollection: public IVariablesCollection
	{
		private:
			std::map<std::pair<String, String>, Any> m_Items;

		protected:
			// IVariablesCollection
			size_t DoGetItemCount(const String& ns) const override;
			size_t DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const override;

			bool DoHasItem(const String& ns, const String& id) const override
			{
				return m_Items.find({ns, id}) != m_Items.end();
			}
			Any DoGetItem(const String& ns, const String& id) const override
			{
				auto it = m_Items.find({ns, id});
				if (it != m_Items.end())
				{
					return it->second;
				}
				return {};
			}
			void DoSetItem(const String& ns, const String& id, Any item) override
			{
				m_Items.insert_or_assign({ns, id}, std::move(item));
			}
	};
}
