#pragma once
#include "KxFramework/KxFramework.h"

enum KxDataViewCtrlStyles: int
{
	KxDV_NONE = 0,
	KxDV_SINGLE_SELECTION = KxDV_NONE,
	KxDV_MULTIPLE_SELECTION = 1 << 0,

	KxDV_NO_HEADER = 1 << 1,
	KxDV_NO_TIMEOUT_EDIT = 1 << 2,
	KxDV_HORIZ_RULES = 1 << 3,
	KxDV_VERT_RULES = 1 << 4,

	KxDV_ALTERNATING_ROW_COLORS = 1 << 5,
	KxDV_VARIABLE_ROW_HEIGHT = 1 << 6,
	KxDV_ITEM_TOOLTIPS = 1 << 7,
	KxDV_NO_COLUMN_AUTO_SIZE = 1 << 8,
	KxDV_DOUBLE_CLICK_EXPAND = 1 << 9,
	KxDV_NO_HOT_TRACK = 1 << 10,
};
constexpr inline KxDataViewCtrlStyles operator|(KxDataViewCtrlStyles v1, KxDataViewCtrlStyles v2)
{
	return static_cast<KxDataViewCtrlStyles>((int)v1 | (int)v2);
}

enum KxDataViewCellMode: int
{
	KxDATAVIEW_CELL_ANY = -1,

	KxDATAVIEW_CELL_INERT = 0,
	KxDATAVIEW_CELL_ACTIVATABLE = 1 << 0,
	KxDATAVIEW_CELL_EDITABLE = 1 << 1
};
constexpr inline KxDataViewCellMode operator|(KxDataViewCellMode v1, KxDataViewCellMode v2)
{
	return static_cast<KxDataViewCellMode>((int)v1 | (int)v2);
}

enum KxDataViewCellState: int
{
	KxDATAVIEW_CELL_NONE = 0,
	KxDATAVIEW_CELL_SELECTED = 1 << 0,
	KxDATAVIEW_CELL_HIGHLIGHTED = 1 << 1,
	KxDATAVIEW_CELL_DROP_TARGET = 1 << 2,
};
constexpr inline KxDataViewCellState operator|(KxDataViewCellState v1, KxDataViewCellState v2)
{
	return static_cast<KxDataViewCellState>((int)v1 | (int)v2);
}

enum KxDataViewColumnFlags
{
	KxDV_COL_NONE = 0,
	KxDV_COL_RESIZEABLE = wxCOL_RESIZABLE,
	KxDV_COL_REORDERABLE = wxCOL_REORDERABLE,
	KxDV_COL_SORTABLE = wxCOL_SORTABLE,
	KxDV_COL_HIDDEN = wxCOL_HIDDEN,

	KxDV_COL_DEFAULT_FLAGS = (int)KxDV_COL_RESIZEABLE | (int)KxDV_COL_REORDERABLE,
};
constexpr inline KxDataViewColumnFlags operator|(KxDataViewColumnFlags v1, KxDataViewColumnFlags v2)
{
	return static_cast<KxDataViewColumnFlags>((int)v1 | (int)v2);
}

enum: int
{
	KxCOL_WIDTH_DEFAULT = -1,
	KxCOL_WIDTH_AUTOSIZE = -2,
};

enum KxDataViewDefaultRowHeightType: int
{
	KxDVC_ROW_HEIGHT_DEFAULT = 0,
	KxDVC_ROW_HEIGHT_EXPLORER,
	KxDVC_ROW_HEIGHT_LISTVIEW,
};

enum: int
{
	// Size of a KxDataViewRenderer without contents
	KxDVC_DEFAULT_RENDERER_SIZE = 20,

	// The default width of new columns
	KxDVC_DEFAULT_WIDTH = 80,

	// The default width of new toggle columns
	KxDVC_TOGGLE_DEFAULT_WIDTH = 30,

	// The default minimal width of the columns
	KxDVC_DEFAULT_MINWIDTH = 30,
};
