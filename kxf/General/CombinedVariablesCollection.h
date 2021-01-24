#pragma once
#include "Common.h"
#include "IVariablesCollection.h"
#include "kxf/Utility/Container.h"

namespace kxf
{
	class KX_API CombinedVariablesCollection final: public IVariablesCollection
	{
		private:
			std::vector<IVariablesCollection*> m_Items;
			IVariablesCollection* m_EditableCollection = nullptr;

		protected:
			// IVariablesCollection
			size_t DoClearItems(const String& ns) override;
			size_t DoGetItemCount(const String& ns) const override;
			size_t DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const override;

			bool DoHasItem(const String& ns, const String& id) const override;
			Any DoGetItem(const String& ns, const String& id) const override;
			void DoSetItem(const String& ns, const String& id, Any item) override;

		public:
			CombinedVariablesCollection(IVariablesCollection& editableCollection, size_t initialCount = 1)
				:m_EditableCollection(&editableCollection)
			{
				m_Items.reserve(std::max<size_t>(initialCount, 1));
				m_Items.emplace_back(&editableCollection);
			}
			CombinedVariablesCollection(const CombinedVariablesCollection&) = default;
			CombinedVariablesCollection(CombinedVariablesCollection&&) = delete;

		public:
			void AddCollection(IVariablesCollection& collection)
			{
				m_Items.emplace_back(&collection);
			}
			void RemoveCollection(const IVariablesCollection& collection)
			{
				Utility::Container::RemoveEachIf(m_Items, [&](IVariablesCollection* item)
				{
					return item == &collection;
				});
			}
			IVariablesCollection& GetEditableCollection()
			{
				return *m_EditableCollection;
			}

		public:
			CombinedVariablesCollection& operator=(const CombinedVariablesCollection&) = default;
			CombinedVariablesCollection& operator=(CombinedVariablesCollection&&) = delete;
	};
}
