#include "stdafx.h"
#include "CellState.h"
#include "MainWindow.h"

namespace kxf::UI::DataView
{
	FlagSet<NativeWidgetFlag> CellState::ToItemState(const MainWindow* window) const
	{
		FlagSet<NativeWidgetFlag> itemState;
		itemState.Add(NativeWidgetFlag::Focused, window->HasFocus());
		itemState.Add(NativeWidgetFlag::Selected, IsSelected());
		itemState.Add(NativeWidgetFlag::Current|NativeWidgetFlag::Focused, IsHotTracked());

		return itemState;
	}
}
