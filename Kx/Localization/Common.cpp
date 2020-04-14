#include "KxStdAfx.h"
#include "Common.h"
#include "Private/StandardLocalization.h"

namespace KxFramework::Localization
{
	String GetStandardLocalizedString(int id)
	{
		return Private::LocalizeLabelString(FromInt<StandardID>(id));
	}
	String GetStandardLocalizedString(StandardID id)
	{
		return Private::LocalizeLabelString(id);
	}
}
