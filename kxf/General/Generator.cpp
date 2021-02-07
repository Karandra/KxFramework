#include "stdafx.h"
#include "Generator.h"

namespace kxf::Private
{
	bool GeneratorCommon::DoMoveNext(bool dryRun) noexcept
	{
		if (m_Index == npos || m_Index >= m_TotalCount)
		{
			return false;
		}

		if ((dryRun && m_TotalCount != npos) || InvokeGenerator(m_Index))
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
	size_t GeneratorCommon::DoDryRun() noexcept
	{
		size_t count = 0;
		if (IsTotalCountKnown())
		{
			count = m_TotalCount;
		}
		else
		{
			while (DoMoveNext(true))
			{
				count++;
			};
		}

		m_IsReset = true;
		return count;
	}
}
