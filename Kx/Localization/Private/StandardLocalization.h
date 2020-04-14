#pragma once
#include "../Common.h"

namespace KxFramework::Localization::Private
{
	String LocalizeLabelString(StandardID id, wxStockLabelQueryFlag flags = wxStockLabelQueryFlag::wxSTOCK_NOFLAGS);
	String LocalizeHelpString(StandardID id, wxStockHelpStringClient client);
}
