#include "KxStdAfx.h"
#include "Common.h"

namespace KxDataView2
{
	ColumnWidth::ColumnWidth(int value) noexcept
		:m_Value(value)
	{
		if (!IsSpecialValue())
		{
			if (m_Value < 0 || m_Value > wxSystemSettings::GetMetric(wxSYS_SCREEN_X))
			{
				m_Value = Value::Default;
			}
		}
	}
}
