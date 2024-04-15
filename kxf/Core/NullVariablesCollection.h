#pragma once
#include "Common.h"
#include "IVariablesCollection.h"
#include "kxf/Utility/Container.h"

namespace kxf
{
	class NullVariablesCollection final: public IVariablesCollection
	{
		protected:
			// IVariablesCollection
			size_t DoClearItems(const String& ns) override
			{
				return 0;
			}
			size_t DoGetItemCount(const String& ns) const override
			{
				return 0;
			}
			size_t DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const override
			{
				return 0;
			}

			bool DoHasItem(const String& ns, const String& id) const override
			{
				return false;
			}
			Any DoGetItem(const String& ns, const String& id) const override
			{
				return {};
			}
			void DoSetItem(const String& ns, const String& id, Any item) override
			{
			}

		public:
			NullVariablesCollection() = default;
	};
}
