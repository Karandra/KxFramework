#include "stdafx.h"
#include "CellState.h"
#include "MainWindow.h"

namespace kxf::UI::DataView
{
	int CellState::ToItemState(const MainWindow* window) const
	{
		int itemState = 0;
		if (window->HasFocus())
		{
			itemState |= wxCONTROL_FOCUSED;
		}
		if (IsSelected())
		{
			itemState |= wxCONTROL_SELECTED;
		}
		if (IsHotTracked())
		{
			itemState |= wxCONTROL_CURRENT|wxCONTROL_FOCUSED;
		}

		return itemState;
	}
}
