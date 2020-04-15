#include "KxStdAfx.h"
#include "Common.h"
#include "Private/StandardLocalization.h"

namespace KxFramework::Localization
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
