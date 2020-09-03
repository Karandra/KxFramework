#pragma once
#include "Common.h"
#include "Any.h"
#include "String.h"
#include "OptionalPtr.h"
#include "kxf/RTTI/QueryInterface.h"

namespace kxf
{
	class KX_API IVariablesCollection: public RTTI::Interface<IVariablesCollection>
	{
		KxDeclareIID(IVariablesCollection, {0xc7cbd77f, 0x4982, 0x4a5f, {0xa6, 0xac, 0x57, 0xbb, 0x3, 0xb6, 0xb3, 0x3b}});

		public:
			virtual ~IVariablesCollection() = default;

		public:
			virtual size_t GetItemCount() const = 0;
			bool IsEmpty() const
			{
				return GetItemCount() == 0;
			}

			virtual String Expand(const String& variables) const = 0;
			virtual size_t EnumItems(std::function<bool(Any)> func) const = 0;

			virtual bool HasItem(const String& id) const = 0;
			virtual Any GetItem(const String& id) const = 0;
			virtual void SetItem(const String& id, Any item) = 0;
	};
}

namespace kxf
{
	String ExpandVariables(const IVariablesCollection& collection, const String& source, std::function<String(const String& ns, const String& name)> onVariable);

	String ExpandStdVariables(const String& source);
	String ExpandStdVariables(const IVariablesCollection& collection, const String& source);
}
