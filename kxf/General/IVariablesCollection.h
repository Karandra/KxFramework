#pragma once
#include "Common.h"
#include "Any.h"
#include "String.h"
#include "OptionalPtr.h"
#include "kxf/RTTI/RTTI.h"

namespace kxf
{
	class IVariablesCollection;
}

namespace kxf
{
	String ExpandVariables(const String& source, std::function<String(const String& ns, const String& id)> onVariable);
	String ExpandVariables(const String& source, const IVariablesCollection& collection);

	String ExpandStdVariables(const String& source);
	String ExpandStdVariables(const String& source, const IVariablesCollection& collection);
}

namespace kxf
{
	class KX_API IVariablesCollection: public RTTI::Interface<IVariablesCollection>
	{
		KxRTTI_DeclareIID(IVariablesCollection, {0xc7cbd77f, 0x4982, 0x4a5f, {0xa6, 0xac, 0x57, 0xbb, 0x3, 0xb6, 0xb3, 0x3b}});

		protected:
			virtual size_t DoClearItems(const String& ns) = 0;
			virtual size_t DoGetItemCount(const String& ns) const = 0;
			virtual size_t DoEnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const = 0;
			virtual String DoExpand(const String& variables) const
			{
				return ExpandVariables(variables, *this);
			}

			virtual bool DoHasItem(const String& ns, const String& id) const = 0;
			virtual Any DoGetItem(const String& ns, const String& id) const = 0;
			virtual void DoSetItem(const String& ns, const String& id, Any item) = 0;

		public:
			virtual ~IVariablesCollection() = default;

		public:
			size_t GetItemCount() const
			{
				return DoGetItemCount({});
			}
			size_t GetItemCount(const String& ns) const
			{
				return DoGetItemCount(ns);
			}

			size_t ClearItems()
			{
				return DoClearItems({});
			}
			size_t ClearItems(const String& ns)
			{
				return DoClearItems(ns);
			}
			bool IsEmpty() const
			{
				return DoGetItemCount({}) == 0;
			}

			String Expand(const String& variables) const
			{
				return DoExpand(variables);
			}
			size_t EnumItems(std::function<bool(const String& ns, const String& id, Any value)> func) const
			{
				return DoEnumItems(std::move(func));
			}

			bool HasItem(const String& id) const
			{
				return DoHasItem({}, id);
			}
			bool HasItem(const String& ns, const String& id) const
			{
				return DoHasItem(ns, id);
			}

			Any GetItem(const String& id) const
			{
				return DoGetItem({}, id);
			}
			Any GetItem(const String& ns, const String& id) const
			{
				return DoGetItem(ns, id);
			}

			void SetItem(const String& id, Any item)
			{
				DoSetItem({}, id, std::move(item));
			}
			void SetItem(const String& ns, const String& id, Any item)
			{
				DoSetItem(ns, id, std::move(item));
			}
	};
}
