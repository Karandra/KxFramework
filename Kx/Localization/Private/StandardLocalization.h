#pragma once
#include "../Common.h"
#include <wx/stockitem.h>

namespace kxf::Localization::Private
{
	String LocalizeLabelString(StdID id, wxStockLabelQueryFlag flags = wxStockLabelQueryFlag::wxSTOCK_NOFLAGS);
	String LocalizeHelpString(StdID id, wxStockHelpStringClient client);
}
