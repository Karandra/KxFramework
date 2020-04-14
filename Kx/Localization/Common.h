#pragma once
#include "Kx/Common.hpp"
#include "Kx/General/String.h"

namespace KxFramework::Localization
{
	String GetStandardLocalizedString(int id);
	String GetStandardLocalizedString(StandardID id);
}
