#include "stdafx.h"
#include "IID.h"
#include "kxf/General/UniversallyUniqueID.h"

namespace kxf
{
	UniversallyUniqueID IID::ToUniversallyUniqueID() const noexcept
	{
		return m_ID;
	}
}
