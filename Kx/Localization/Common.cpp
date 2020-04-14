#include "KxStdAfx.h"
#include "Common.h"
#include "Kx/General/StringFormater.h"

namespace KxFramework::Localization
{
	String GetStandardLocalizedString(int id)
	{
		return {};
	}
	String GetStandardLocalizedString(StandardID id)
	{
		return GetStandardLocalizedString(ToInt(id));
	}
}
