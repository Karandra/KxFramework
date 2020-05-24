#include "stdafx.h"
#include "Common.h"
#include "Private/StandardLocalization.h"

namespace kxf::Localization
{
	String GetStandardLocalizedString(int id)
	{
		return Private::LocalizeLabelString(FromInt<StdID>(id));
	}
	String GetStandardLocalizedString(StdID id)
	{
		return Private::LocalizeLabelString(id);
	}
}
