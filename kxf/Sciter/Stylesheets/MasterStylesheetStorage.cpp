#include "stdafx.h"
#include "MasterStylesheetStorage.h"
#include "../SciterAPI.h"
#include "../Internal.h"

namespace kxf::Sciter
{
	bool MasterStylesheetStorage::ApplyGlobally() const
	{
		// Reset the master CSS
		constexpr BYTE null[8] = {};
		GetSciterAPI()->SciterSetMasterCSS(null, 0);
		
		// Apply new master styles
		bool success = false;
		EnumItems([&](const String& css)
		{
			auto utf8 = ToSciterUTF8(css);
			if (!GetSciterAPI()->SciterAppendMasterCSS(utf8.data(), utf8.size()))
			{
				success = false;
				return false;
			}
			return true;
		});
		return success;
	}
}
