#pragma once
#include "kxf/General/String.h"

namespace kxf
{
	enum class SoftwareLicenseType
	{
		None = -1,

		MIT,
		ZLib,
		GNU_GPLv3,
		GNU_LGPLv3
	};
}

namespace kxf
{
	class SoftwareLicenseDB final
	{
		public:
			static const SoftwareLicenseDB& Get();

		public:
			String GetName(SoftwareLicenseType licenseType) const;
			String GetText(SoftwareLicenseType licenseType, const String& copyright = {}) const;
	};
}
