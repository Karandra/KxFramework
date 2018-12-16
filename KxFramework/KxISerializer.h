/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxISerializer
{
	public:
		virtual bool Serialize(wxOutputStream& stream) const = 0;
		virtual bool DeSerialize(wxInputStream& stream) = 0;
};
