#include "stdafx.h"
#include "Enumerator.h"

namespace kxf
{
	bool AbstractEnumerator::DoMoveNext() noexcept
	{
		if (m_Index == npos || m_Index >= m_TotalCount)
		{
			return false;
		}

		if (InvokeGenerator())
		{
			m_Index++;
			return true;
		}
		else
		{
			m_Index = npos;
			return false;
		}
	}
}
