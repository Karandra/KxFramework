#pragma once
#include "EnumClass.h"
#include "Kx/System/UndefWindows.h"
#include <wx/defs.h>

namespace KxFramework
{
	enum class StandardID: int
	{
		// wxWidgets IDs
		WX_AUTO_LOWEST = wxID_AUTO_LOWEST,
		WX_AUTO_HIGHEST = wxID_AUTO_HIGHEST,

		// All predefined ids are between wxID_LOWEST and wxID_HIGHEST
		WX_LOWEST = wxID_LOWEST,

		// Highest wxWidgets generic ID
		WX_HIGHEST = wxID_HIGHEST,

		// No ID matches this one when compared to it
		None = wxID_NONE,

		// ID for a separator line in the menu (invalid for normal item)
		Separator = wxID_SEPARATOR,

		// Any ID means that we don't care about the ID, whether when installing an event handler or when creating a new window.
		Any = wxID_ANY,

		Open = wxID_OPEN,
		Close = wxID_CLOSE,
		New = wxID_NEW,
		Save = wxID_SAVE,
		SaveAs = wxID_SAVEAS,
		Revert = wxID_REVERT,
		Exit = wxID_EXIT,
		Undo = wxID_UNDO,
		Redo = wxID_REDO,
		Help = wxID_HELP,
		Print = wxID_PRINT,
		PrintSetup = wxID_PRINT_SETUP,
		PageSetup = wxID_PAGE_SETUP,
		Preview = wxID_PREVIEW,
		About = wxID_ABOUT,
		HelpContents = wxID_HELP_CONTENTS,
		HelpIndex = wxID_HELP_INDEX,
		HelpSearch = wxID_HELP_SEARCH,
		HelpCommands = wxID_HELP_COMMANDS,
		HelpProcedures = wxID_HELP_PROCEDURES,
		HelpContext = wxID_HELP_CONTEXT,
		CloseAll = wxID_CLOSE_ALL,
		Preferences = wxID_PREFERENCES,

		Edit = wxID_EDIT,
		Cut = wxID_CUT,
		Copy = wxID_COPY,
		Paste = wxID_PASTE,
		Clear = wxID_CLEAR,
		Find = wxID_FIND,
		Duplicate = wxID_DUPLICATE,
		SelectAll = wxID_SELECTALL,
		Delete = wxID_DELETE,
		Replace = wxID_REPLACE,
		ReplaceAll = wxID_REPLACE_ALL,
		Properties = wxID_PROPERTIES,

		ViewDetails = wxID_VIEW_DETAILS,
		ViewLargeIcons = wxID_VIEW_LARGEICONS,
		ViewSmallIcons = wxID_VIEW_SMALLICONS,
		ViewList = wxID_VIEW_LIST,
		ViewSortDate = wxID_VIEW_SORTDATE,
		ViewSortName = wxID_VIEW_SORTNAME,
		ViewSortSize = wxID_VIEW_SORTSIZE,
		ViewSortType = wxID_VIEW_SORTTYPE,

		File = wxID_FILE,
		File1 = wxID_FILE1,
		File2 = wxID_FILE2,
		File3 = wxID_FILE3,
		File4 = wxID_FILE4,
		File5 = wxID_FILE5,
		File6 = wxID_FILE6,
		File7 = wxID_FILE7,
		File8 = wxID_FILE8,
		File9 = wxID_FILE9,
		File10,

		// Standard button and menu IDs
		OK = wxID_OK,
		Cancel = wxID_CANCEL,
		Apply = wxID_APPLY,
		Yes = wxID_YES,
		No = wxID_NO,
		Static = wxID_STATIC,
		Forward = wxID_FORWARD,
		Backward = wxID_BACKWARD,
		Default = wxID_DEFAULT,
		More = wxID_MORE,
		Setup = wxID_SETUP,
		Reset = wxID_RESET,
		ContextHelp = wxID_CONTEXT_HELP,
		YesToAll = wxID_YESTOALL,
		NoToAll = wxID_NOTOALL,
		Abort = wxID_ABORT,
		Retry = wxID_RETRY,
		Ignore = wxID_IGNORE,
		Add = wxID_ADD,
		Remove = wxID_REMOVE,

		Up = wxID_UP,
		Down = wxID_DOWN,
		Home = wxID_HOME,
		Refresh = wxID_REFRESH,
		Stop = wxID_STOP,
		Index = wxID_INDEX,

		Bold = wxID_BOLD,
		Italic = wxID_ITALIC,
		JustifyCenter = wxID_JUSTIFY_CENTER,
		JustifyFill = wxID_JUSTIFY_FILL,
		JustifyRight = wxID_JUSTIFY_RIGHT,
		JustifyLeft = wxID_JUSTIFY_LEFT,
		Underline = wxID_UNDERLINE,
		Indent = wxID_INDENT,
		Unindent = wxID_UNINDENT,
		Zoom100 = wxID_ZOOM_100,
		ZoomFit = wxID_ZOOM_FIT,
		ZoomIn = wxID_ZOOM_IN,
		ZoomOut = wxID_ZOOM_OUT,
		Undelete = wxID_UNDELETE,
		RevertToSaved = wxID_REVERT_TO_SAVED,
		CDROM = wxID_CDROM,
		Convert = wxID_CONVERT,
		Execute = wxID_EXECUTE,
		Floppy = wxID_FLOPPY,
		HardDisk = wxID_HARDDISK,
		Bottom = wxID_BOTTOM,
		First = wxID_FIRST,
		Last = wxID_LAST,
		Top = wxID_TOP,
		Info = wxID_INFO,
		JumpTo = wxID_JUMP_TO,
		Network = wxID_NETWORK,
		SelectColor = wxID_SELECT_COLOR,
		SelectFont = wxID_SELECT_FONT,
		SortAscending = wxID_SORT_ASCENDING,
		SortDescending = wxID_SORT_DESCENDING,
		SpellCheck = wxID_SPELL_CHECK,
		Strikethrough = wxID_STRIKETHROUGH,

		SystemMenu = wxID_SYSTEM_MENU,
		CloseFrame = wxID_CLOSE_FRAME,
		MoveFrame = wxID_MOVE_FRAME,
		ResizeFrame = wxID_RESIZE_FRAME,
		MaximizeFrame = wxID_MAXIMIZE_FRAME,
		MinimizeFrame = wxID_ICONIZE_FRAME,
		RestoreFrame = wxID_RESTORE_FRAME,

		MDIWindowCascade = wxID_MDI_WINDOW_CASCADE,
		MDIWindowTileHorizontal = wxID_MDI_WINDOW_TILE_HORZ,
		MDIWindowTileVertical = wxID_MDI_WINDOW_TILE_VERT,
		MDIWindowArrangeIcons = wxID_MDI_WINDOW_ARRANGE_ICONS,
		MDIWindowPrevious = wxID_MDI_WINDOW_PREV,
		MDIWindowNext = wxID_MDI_WINDOW_NEXT,
		MDIWindowFirst = wxID_MDI_WINDOW_FIRST,
		MDIWindowLast = wxID_MDI_WINDOW_LAST,

		// IDs used by generic file dialog (13 consecutive starting from this value)
		FileDialog = wxID_FILEDLGG,

		// IDs used by generic file control (4 consecutive starting from this value)
		FileControl = wxID_FILECTRL,

		// KxFramework IDs
		KX_LOWEST = wxID_HIGHEST + 1,

		Move,
		Error,
		Rename,
		Directory,
		SelectFile,
		SelectDirectory,
		Search,
		CopyLink,
		CopyAddress,

		KX_HIGHEST
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowCast(StandardID);
	}
}

namespace KxFramework
{
	constexpr inline StandardID FromWxStandardID(int stdWxID) noexcept
	{
		return FromInt<StandardID>(stdWxID);
	}
	constexpr inline int ToWxStandardID(StandardID stdID) noexcept
	{
		return ToInt(stdID);
	}
}
