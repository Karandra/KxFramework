#pragma once
#include "Common.h"
#include "StaticVariablesCollection.h"

namespace kxf
{
	class KX_API DynamicVariablesCollection: public StaticVariablesCollection
	{
		public:
			using TValue = std::function<Any(const String& ns, const String& id)>;

		private:
			std::map<std::pair<String, String>, TValue> m_DynamicItems;

		protected:
			// IVariablesCollection
			size_t DoGetItemCount(const String& ns) const override;
			size_t DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const override;

			bool DoHasItem(const String& ns, const String& id) const override;
			Any DoGetItem(const String& ns, const String& id) const override;
			void DoSetItem(const String& ns, const String& id, Any item) override;

			// DynamicVariablesCollection
			void DoSetDynamicItem(const String& ns, const String& id, TValue func)
			{
				m_DynamicItems.insert_or_assign({ns, id}, std::move(func));
			}

		public:
			// DynamicVariablesCollection
			void SetDynamicItem(const String& id, TValue func)
			{
				DoSetDynamicItem({}, id, std::move(func));
			}
			void SetDynamicItem(const String& ns, const String& id, TValue func)
			{
				DoSetDynamicItem(ns, id, std::move(func));
			}

			void SetDynamicItem(const String& id, std::function<Any(void)> func)
			{
				DoSetDynamicItem({}, id, [func = std::move(func)](const String&, const String&)
				{
					return std::invoke(func);
				});
			}
			void SetDynamicItem(const String& ns, const String& id, std::function<Any(void)> func)
			{
				DoSetDynamicItem(ns, id, [func = std::move(func)](const String&, const String&)
				{
					return std::invoke(func);
				});
			}
	};
}
