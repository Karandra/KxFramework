#include "KxStdAfx.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxColor.h"
#include "KxFramework/KxTranslation.h"
#include "KxFramework/KxIncludeWindows.h"

const wxRegion KxNullWxRegion = wxRegion();
const wxRect KxNullWxRect = wxRect();
const wxString KxNullWxString = wxString();

namespace
{
	const wxString LoadResourceAux(HRSRC resourceHandle, HMODULE moduleHandle)
	{
		DWORD size = ::SizeofResource(moduleHandle, resourceHandle);
		if (size != 0)
		{
			HGLOBAL resHandle = ::LoadResource(moduleHandle, resourceHandle);
			void* data = ::LockResource(resHandle);
			if (data)
			{
				return wxString::From8BitData((const char*)data, size);
			}
		}
		return wxEmptyString;
	}

	wxString wxGetStockLabelAux(wxString&& id, long flags, const wxChar* defaultValue = nullptr, bool* isSuccessOut = nullptr)
	{
		if (flags & wxSTOCK_WITH_MNEMONIC)
		{
			id.Append(wxS("_MM"));
		}

		bool isSuccess = false;
		wxString value = KxTranslation::GetCurrent().GetString(id, &isSuccess);
		if (!isSuccess && flags & wxSTOCK_WITH_MNEMONIC)
		{
			id.RemoveLast(3);
			value = KxTranslation::GetCurrent().GetString(id, &isSuccess);
		}

		KxUtility::SetIfNotNull(isSuccessOut, isSuccess);
		if (isSuccess)
		{
			return value;
		}
		else if (defaultValue)
		{
			KxUtility::SetIfNotNull(isSuccessOut, true);
			return defaultValue;
		}
		else
		{
			return id;
		}
	}
	wxString wxGetStockLabelLocalized(wxWindowID id, long flags, bool* isSuccess)
	{
		wxString stockLabel;
		// special case: the "Cancel" button shouldn't have a mnemonic under MSW
		// for consistency with the native dialogs (which don't use any mnemonic
		// for it because it is already bound to Esc implicitly)
		if (id == wxID_CANCEL)
		{
			flags &= ~wxSTOCK_WITH_MNEMONIC;
		}

		#define STOCKITEM_WX(stockid, defaultLabel)														\
		case wx##stockid:																			\
		{																							\
			stockLabel = wxGetStockLabelAux(wxS(#stockid), flags, wxS(defaultLabel), isSuccess);	\
			break;																					\
		}

		#define STOCKITEM_KX(stockid, defaultLabel)														\
		case Kx##stockid:																			\
		{																							\
			stockLabel = wxGetStockLabelAux(wxS(#stockid), flags, wxS(defaultLabel), isSuccess);	\
			break;																					\
		}

		switch (id)
		{
			STOCKITEM_WX(ID_ABOUT, "About");
			STOCKITEM_WX(ID_ADD, "Add");
			STOCKITEM_WX(ID_APPLY, "Apply");
			STOCKITEM_WX(ID_BACKWARD, "Back");
			STOCKITEM_WX(ID_IGNORE, "Ignore");
			STOCKITEM_WX(ID_BOLD, "Bold");
			STOCKITEM_WX(ID_BOTTOM, "Bottom");
			STOCKITEM_WX(ID_CANCEL, "Cancel");
			STOCKITEM_WX(ID_CDROM, "CD-ROM");
			STOCKITEM_WX(ID_CLEAR, "Clear");
			STOCKITEM_WX(ID_CLOSE, "Close");
			STOCKITEM_WX(ID_CONVERT, "Convert");
			STOCKITEM_WX(ID_COPY, "Copy");
			STOCKITEM_WX(ID_CUT, "Cut");
			STOCKITEM_WX(ID_DELETE, "Delete");
			STOCKITEM_WX(ID_DOWN, "Down");
			STOCKITEM_WX(ID_EDIT, "Edit");
			STOCKITEM_WX(ID_EXECUTE, "Execute");
			STOCKITEM_WX(ID_EXIT, "Quit");
			STOCKITEM_WX(ID_FILE, "File");
			STOCKITEM_WX(ID_FIND, "Find");
			STOCKITEM_WX(ID_FIRST, "First");
			STOCKITEM_WX(ID_FLOPPY, "Floppy");
			STOCKITEM_WX(ID_FORWARD, "Forward");
			STOCKITEM_WX(ID_HARDDISK, "Hard disk");
			STOCKITEM_WX(ID_HELP, "Help");
			STOCKITEM_WX(ID_HOME, "Home");
			STOCKITEM_WX(ID_INDENT, "Indent");
			STOCKITEM_WX(ID_INDEX, "Index");
			STOCKITEM_WX(ID_INFO, "Info");
			STOCKITEM_WX(ID_ITALIC, "Italic");
			STOCKITEM_WX(ID_JUMP_TO, "Jump to");
			STOCKITEM_WX(ID_JUSTIFY_CENTER, "Centered");
			STOCKITEM_WX(ID_JUSTIFY_FILL, "Justified");
			STOCKITEM_WX(ID_JUSTIFY_LEFT, "Align left");
			STOCKITEM_WX(ID_JUSTIFY_RIGHT, "Align right");
			STOCKITEM_WX(ID_LAST, "Last");
			STOCKITEM_WX(ID_NETWORK, "Network");
			STOCKITEM_WX(ID_NEW, "New");
			STOCKITEM_WX(ID_NO, "No");
			STOCKITEM_WX(ID_NONE, "None");
			STOCKITEM_WX(ID_OK, "OK");
			STOCKITEM_WX(ID_OPEN, "Open...");
			STOCKITEM_WX(ID_PASTE, "Paste");
			STOCKITEM_WX(ID_PREFERENCES, "Preferences");
			STOCKITEM_WX(ID_PREVIEW, "Print preview...");
			STOCKITEM_WX(ID_PRINT, "Print...");
			STOCKITEM_WX(ID_PROPERTIES, "Properties");
			STOCKITEM_WX(ID_REDO, "Redo");
			STOCKITEM_WX(ID_REFRESH, "Refresh");
			STOCKITEM_WX(ID_REMOVE, "Remove");
			STOCKITEM_WX(ID_REPLACE, "Replace");
			STOCKITEM_WX(ID_REVERT_TO_SAVED, "Revert to saved");
			STOCKITEM_WX(ID_SAVE, "Save");
			STOCKITEM_WX(ID_SAVEAS, "Save as");
			STOCKITEM_WX(ID_SELECTALL, "Select all");
			STOCKITEM_WX(ID_SELECT_COLOR, "Select color");
			STOCKITEM_WX(ID_SELECT_FONT, "Select font");
			STOCKITEM_WX(ID_SORT_ASCENDING, "Ascending");
			STOCKITEM_WX(ID_SORT_DESCENDING, "Descending");
			STOCKITEM_WX(ID_SPELL_CHECK, "Spell check");
			STOCKITEM_WX(ID_STOP, "Stop");
			STOCKITEM_WX(ID_STRIKETHROUGH, "Strikethrough");
			STOCKITEM_WX(ID_TOP, "Top");
			STOCKITEM_WX(ID_UNDELETE, "Undelete");
			STOCKITEM_WX(ID_UNDERLINE, "Underline");
			STOCKITEM_WX(ID_UNDO, "Undo");
			STOCKITEM_WX(ID_UNINDENT, "Unindent");
			STOCKITEM_WX(ID_UP, "Up");
			STOCKITEM_WX(ID_YES, "Yes");
			STOCKITEM_WX(ID_ZOOM_100, "Actual size");
			STOCKITEM_WX(ID_ZOOM_FIT, "Zoomto fit");
			STOCKITEM_WX(ID_ZOOM_IN, "Zoom in");
			STOCKITEM_WX(ID_ZOOM_OUT, "Zoom out");

			STOCKITEM_KX(ID_MOVE, "Move");
			STOCKITEM_KX(ID_RENAME, "Rename");
			STOCKITEM_KX(ID_ERROR, "Error");
			STOCKITEM_KX(ID_SELECT_FILE, "Select file");
			STOCKITEM_KX(ID_SELECT_FOLDER, "Select folder");
			STOCKITEM_KX(ID_FOLDER, "Folder");
			STOCKITEM_KX(ID_SEARCH, "Search");
			STOCKITEM_KX(ID_COPY_LINK, "Copy link");
		};
		#undef STOCKITEM

		if (flags & wxSTOCK_WITHOUT_ELLIPSIS)
		{
			wxString sBaseLabel;
			if (stockLabel.EndsWith(wxS("..."), &sBaseLabel))
			{
				stockLabel = sBaseLabel;
			}

			// accelerators only make sense for the menu items which should have
			// ellipsis too while wxSTOCK_WITHOUT_ELLIPSIS is mostly useful for
			// buttons which shouldn't have accelerators in their labels
		}

		#if wxUSE_ACCEL
		if (!stockLabel.empty() && (flags & wxSTOCK_WITH_ACCELERATOR))
		{
			wxAcceleratorEntry tAccel = wxGetStockAccelerator(id);
			if (tAccel.IsOk())
			{
				stockLabel << wxS('\t') << tAccel.ToString();
			}
		}
		#endif // wxUSE_ACCEL

		return stockLabel;
	}
	wxString wxGetStockHelpStringLocalized(wxWindowID id, wxStockHelpStringClient client)
	{
		wxString stockHelp;

		#define STOCKITEM(stockid, ctx, defaultLabel)													\
        case wx##stockid:																			\
		{																							\
			if (client == ctx)																		\
			{																						\
				stockHelp = wxGetStockLabelAux(wxS(#stockid) wxS("_LONG"), 0, wxS(defaultLabel));	\
			}																						\
			break;																					\
		}

		switch (id)
		{
			// NB: these help string should be not too specific as they could be used in completely different programs!
			STOCKITEM(ID_ABOUT, wxSTOCK_MENU, "Show about dialog");
			STOCKITEM(ID_COPY, wxSTOCK_MENU, "Copy selection");
			STOCKITEM(ID_CUT, wxSTOCK_MENU, "Cut selection");
			STOCKITEM(ID_DELETE, wxSTOCK_MENU, "Delete selection");
			STOCKITEM(ID_REPLACE, wxSTOCK_MENU, "Replace selection");
			STOCKITEM(ID_PASTE, wxSTOCK_MENU, "Paste selection");
			STOCKITEM(ID_EXIT, wxSTOCK_MENU, "Quit this program");
			STOCKITEM(ID_UNDO, wxSTOCK_MENU, "Undo last action");
			STOCKITEM(ID_REDO, wxSTOCK_MENU, "Redo last action");
			STOCKITEM(ID_CLOSE, wxSTOCK_MENU, "Close current document");
			STOCKITEM(ID_SAVE, wxSTOCK_MENU, "Save current document");
			STOCKITEM(ID_SAVEAS, wxSTOCK_MENU, "Save current document with a different filename");

			default:
			{
				// There's no stock help string for this ID / client
				return wxEmptyString;
			}
		};
		#undef STOCKITEM

		return stockHelp;
	}
}

namespace KxUtility
{
	HMODULE GetAppHandle()
	{
		return GetModuleHandleW(nullptr);
	}
	const wxString LoadResource(const wxString& name, const wxString& typeName)
	{
		HMODULE moduleHandle = GetAppHandle();
		HRSRC resourceHandle = ::FindResourceW(moduleHandle, name.wc_str(), typeName.wc_str());
		return LoadResourceAux(resourceHandle, moduleHandle);
	}
	const wxString LoadResource(int id, const wxString& typeName)
	{
		HMODULE moduleHandle = GetAppHandle();
		HRSRC resourceHandle = ::FindResourceW(moduleHandle, MAKEINTRESOURCEW(id), typeName.wc_str());
		return LoadResourceAux(resourceHandle, moduleHandle);
	}
	void ToggleWindowStyle(HWND hWnd, int index, LONG style, bool enable)
	{
		::SetWindowLongPtrW(hWnd, index, ModFlag(::GetWindowLongPtrW(hWnd, index), style, enable));
	}

	wxString GetStandardLocalizedString(int id, bool* isSuccess)
	{
		return wxGetStockLabelLocalized(id, wxSTOCK_WITHOUT_ELLIPSIS, isSuccess);
	}
	bool StringToBool(const wxString& value, bool* isUnknown)
	{
		if (value.IsSameAs(wxS("false"), false))
		{
			SetIfNotNull(isUnknown, false);
			return false;
		}
		else if (value.IsSameAs(wxS("true"), false))
		{
			SetIfNotNull(isUnknown, false);
			return true;
		}

		long long iValue = 0;
		double fValue = 0;
		if (value.ToLongLong(&iValue))
		{
			SetIfNotNull(isUnknown, false);
			return iValue != 0;
		}
		else if (value.ToCDouble(&fValue))
		{
			SetIfNotNull(isUnknown, false);
			return fValue != 0.0;
		}

		SetIfNotNull(isUnknown, true);
		return false;
	}
}

