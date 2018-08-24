#include "KxStdAfx.h"
#include "KxFramework/KxUtility.h"
#include "KxFramework/KxColor.h"
#include "KxFramework/KxTranslation.h"
#include "KxFramework/KxIncludeWindows.h"

const wxRegion wxNullRegion = wxRegion();
const wxRect wxNullRect = wxRect();
const wxString wxNullString = wxString();

wxString wxGetStockLabelLocalized(wxWindowID id, long flags, bool* isSuccess = NULL);

/* Auxiliary library */
HMODULE KxUtility::GetAppHandle()
{
	return GetModuleHandleW(NULL);
}
const wxString KxUtility::LoadResourceAux(HRSRC resourceHandle, HMODULE moduleHandle)
{
	DWORD size = ::SizeofResource(moduleHandle, resourceHandle);
	if (size != 0)
	{
		HGLOBAL hLoadedResource = ::LoadResource(moduleHandle, resourceHandle);
		void* data = ::LockResource(hLoadedResource);
		if (data)
		{
			return wxString::From8BitData((const char*)data, size);
		}
	}
	return wxEmptyString;
}
const wxString KxUtility::LoadResource(const wxString& name, const wxString& typeName)
{
	HMODULE moduleHandle = GetAppHandle();
	HRSRC resourceHandle = ::FindResourceW(moduleHandle, name.wc_str(), typeName.wc_str());
	return LoadResourceAux(resourceHandle, moduleHandle);
}
const wxString KxUtility::LoadResource(int id, const wxString& typeName)
{
	HMODULE moduleHandle = GetAppHandle();
	HRSRC resourceHandle = ::FindResourceW(moduleHandle, MAKEINTRESOURCEW(id), typeName.wc_str());
	return LoadResourceAux(resourceHandle, moduleHandle);
}
void KxUtility::ToggleWindowStyle(HWND hWnd, int index, LONG style, bool enable)
{
	SetWindowLongPtrW(hWnd, index, ModFlag(GetWindowLongPtrW(hWnd, index), style, enable));
}
wxString KxUtility::GetStandardLocalizedString(int id, bool* isSuccess)
{
	return wxGetStockLabelLocalized(id, wxSTOCK_WITHOUT_ELLIPSIS, isSuccess);
}

bool KxUtility::StringToBool(const wxString& value, bool* isUnknown)
{
	if (value.IsSameAs("false", false))
	{
		SetIfNotNull(isUnknown, false);
		return false;
	}
	else if (value.IsSameAs("true", false))
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

static wxString wxGetStockLabelAux(const char* sCharID, long flags, const char* defaultValue = NULL, bool* isSuccessOut = NULL)
{
	wxString id = wxString::FromUTF8Unchecked(sCharID);

	if (flags & wxSTOCK_WITH_MNEMONIC)
	{
		id.Append("_MM");
	}

	bool bGetSuccess = false;
	wxString value = KxTranslation::GetCurrent().GetString(id, &bGetSuccess);
	if (!bGetSuccess)
	{
		id.RemoveLast(3);
		value = KxTranslation::GetCurrent().GetString(id, &bGetSuccess);
	}

	KxUtility::SetIfNotNull(isSuccessOut, bGetSuccess);
	if (bGetSuccess)
	{
		return value;
	}
	else if (defaultValue)
	{
		KxUtility::SetIfNotNull(isSuccessOut, true);
		return wxString::FromUTF8Unchecked(defaultValue);
	}
	else
	{
		return id;
	}
}
wxString wxGetStockLabelLocalized(wxWindowID id, long flags, bool* isSuccess)
{
	wxString sStockLabel;
	// special case: the "Cancel" button shouldn't have a mnemonic under MSW
	// for consistency with the native dialogs (which don't use any mnemonic
	// for it because it is already bound to Esc implicitly)
	if (id == wxID_CANCEL)
	{
		flags &= ~wxSTOCK_WITH_MNEMONIC;
	}

	#define STOCKITEM_WX(stockid, sDefaultLabel)												\
		case wx##stockid:																		\
		{																						\
			sStockLabel = wxGetStockLabelAux(u8## #stockid, flags, sDefaultLabel, isSuccess);	\
			break;																				\
		}

	#define STOCKITEM_KX(stockid, sDefaultLabel)												\
		case Kx##stockid:																		\
		{																						\
			sStockLabel = wxGetStockLabelAux(u8## #stockid, flags, sDefaultLabel, isSuccess);	\
			break;																				\
		}
	
	switch (id)
	{
		STOCKITEM_WX(ID_ABOUT, u8"About");
		STOCKITEM_WX(ID_ADD, u8"Add");
		STOCKITEM_WX(ID_APPLY, u8"Apply");
		STOCKITEM_WX(ID_BACKWARD, u8"Back");
		STOCKITEM_WX(ID_IGNORE, u8"Ignore");
		STOCKITEM_WX(ID_BOLD, u8"Bold");
		STOCKITEM_WX(ID_BOTTOM, u8"Bottom");
		STOCKITEM_WX(ID_CANCEL, u8"Cancel");
		STOCKITEM_WX(ID_CDROM, u8"CD-ROM");
		STOCKITEM_WX(ID_CLEAR, u8"Clear");
		STOCKITEM_WX(ID_CLOSE, u8"Close");
		STOCKITEM_WX(ID_CONVERT, u8"Convert");
		STOCKITEM_WX(ID_COPY, u8"Copy");
		STOCKITEM_WX(ID_CUT, u8"Cut");
		STOCKITEM_WX(ID_DELETE, u8"Delete");
		STOCKITEM_WX(ID_DOWN, u8"Down");
		STOCKITEM_WX(ID_EDIT, u8"Edit");
		STOCKITEM_WX(ID_EXECUTE, u8"Execute");
		STOCKITEM_WX(ID_EXIT, u8"Quit");
		STOCKITEM_WX(ID_FILE, u8"File");
		STOCKITEM_WX(ID_FIND, u8"Find");
		STOCKITEM_WX(ID_FIRST, u8"First");
		STOCKITEM_WX(ID_FLOPPY, u8"Floppy");
		STOCKITEM_WX(ID_FORWARD, u8"Forward");
		STOCKITEM_WX(ID_HARDDISK, u8"Hard disk");
		STOCKITEM_WX(ID_HELP, u8"Help");
		STOCKITEM_WX(ID_HOME, u8"Home");
		STOCKITEM_WX(ID_INDENT, u8"Indent");
		STOCKITEM_WX(ID_INDEX, u8"Index");
		STOCKITEM_WX(ID_INFO, u8"Info");
		STOCKITEM_WX(ID_ITALIC, u8"Italic");
		STOCKITEM_WX(ID_JUMP_TO, u8"Jump to");
		STOCKITEM_WX(ID_JUSTIFY_CENTER, u8"Centered");
		STOCKITEM_WX(ID_JUSTIFY_FILL, u8"Justified");
		STOCKITEM_WX(ID_JUSTIFY_LEFT, u8"Align left");
		STOCKITEM_WX(ID_JUSTIFY_RIGHT, u8"Align right");
		STOCKITEM_WX(ID_LAST, u8"Last");
		STOCKITEM_WX(ID_NETWORK, u8"Network");
		STOCKITEM_WX(ID_NEW, u8"New");
		STOCKITEM_WX(ID_NO, u8"No");
		STOCKITEM_WX(ID_NONE, u8"None");
		STOCKITEM_WX(ID_OK, u8"OK");
		STOCKITEM_WX(ID_OPEN, u8"Open...");
		STOCKITEM_WX(ID_PASTE, u8"Paste");
		STOCKITEM_WX(ID_PREFERENCES, u8"Preferences");
		STOCKITEM_WX(ID_PREVIEW, u8"Print preview...");
		STOCKITEM_WX(ID_PRINT, u8"Print...");
		STOCKITEM_WX(ID_PROPERTIES, u8"Properties");
		STOCKITEM_WX(ID_REDO, u8"Redo");
		STOCKITEM_WX(ID_REFRESH, u8"Refresh");
		STOCKITEM_WX(ID_REMOVE, u8"Remove");
		STOCKITEM_WX(ID_REPLACE, u8"Replace");
		STOCKITEM_WX(ID_REVERT_TO_SAVED, u8"Revert to saved");
		STOCKITEM_WX(ID_SAVE, u8"Save");
		STOCKITEM_WX(ID_SAVEAS, u8"Save as");
		STOCKITEM_WX(ID_SELECTALL, u8"Select all");
		STOCKITEM_WX(ID_SELECT_COLOR, u8"Select color");
		STOCKITEM_WX(ID_SELECT_FONT, u8"Select font");
		STOCKITEM_WX(ID_SORT_ASCENDING, u8"Ascending");
		STOCKITEM_WX(ID_SORT_DESCENDING, u8"Descending");
		STOCKITEM_WX(ID_SPELL_CHECK, u8"Spell check");
		STOCKITEM_WX(ID_STOP, u8"Stop");
		STOCKITEM_WX(ID_STRIKETHROUGH, u8"Strikethrough");
		STOCKITEM_WX(ID_TOP, u8"Top");
		STOCKITEM_WX(ID_UNDELETE, u8"Undelete");
		STOCKITEM_WX(ID_UNDERLINE, u8"Underline");
		STOCKITEM_WX(ID_UNDO, u8"Undo");
		STOCKITEM_WX(ID_UNINDENT, u8"Unindent");
		STOCKITEM_WX(ID_UP, u8"Up");
		STOCKITEM_WX(ID_YES, u8"Yes");
		STOCKITEM_WX(ID_ZOOM_100, u8"Actual size");
		STOCKITEM_WX(ID_ZOOM_FIT, u8"Zoomto fit");
		STOCKITEM_WX(ID_ZOOM_IN, u8"Zoom in");
		STOCKITEM_WX(ID_ZOOM_OUT, u8"Zoom out");

		STOCKITEM_KX(ID_MOVE, u8"Move");
		STOCKITEM_KX(ID_RENAME, u8"Rename");
		STOCKITEM_KX(ID_ERROR, u8"Error");
		STOCKITEM_KX(ID_SELECT_FILE, u8"Select file");
		STOCKITEM_KX(ID_SELECT_FOLDER, u8"Select folder");
		STOCKITEM_KX(ID_FOLDER, u8"Folder");
		STOCKITEM_KX(ID_SEARCH, u8"Search");
	};
	#undef STOCKITEM

	/*
	#define STOCKITEM(stockid, labelWithMnemonic, labelPlain)                 \
		case stockid:                                                         \
			if(flags & wxSTOCK_WITH_MNEMONIC)                                 \
				stockLabel = wxString::FromUTF8Unchecked(labelWithMnemonic);  \
			else                                                              \
				stockLabel = wxString::FromUTF8Unchecked(labelPlain);         \
			break
	

	switch (id)
	{
		STOCKITEM(wxID_ABOUT, "&О программе", "О программе");
		STOCKITEM(wxID_ADD, "Добавить", "Добавить");
		STOCKITEM(wxID_APPLY, "&Применить", "Применить");
		STOCKITEM(wxID_BACKWARD, "&Назад", "Назад");
		STOCKITEM(wxID_BOLD, "&Жирный", "Жирный");
		STOCKITEM(wxID_BOTTOM, "&Низ", "Низ");
		STOCKITEM(wxID_CANCEL, "&Отмена", "Отмена");
		STOCKITEM(wxID_CDROM, "&CD-ROM", "CD-ROM");
		STOCKITEM(wxID_CLEAR, "&Очистить", "Очистить");
		STOCKITEM(wxID_CLOSE, "&Закрыть", "Закрыть");
		STOCKITEM(wxID_CONVERT, "&Конвертировать", "Конвертировать");
		STOCKITEM(wxID_COPY, "&Копировать", "Копировать");
		STOCKITEM(wxID_CUT, "&Вырезать", "Вырезать");
		STOCKITEM(wxID_DELETE, "&Удалить", "Удалить");
		STOCKITEM(wxID_DOWN, "&Вниз", "Вниз");
		STOCKITEM(wxID_EDIT, "&Изименить", "Изименить");
		STOCKITEM(wxID_EXECUTE, "&Выполнить", "Выполнить");
		STOCKITEM(wxID_EXIT, "&Выход", "Выход");
		STOCKITEM(wxID_FILE, "&Файл", "Файл");
		STOCKITEM(wxID_FIND, "&Поиск", "Поиск");
		STOCKITEM(wxID_FIRST, "&Первый", "Первый");
		STOCKITEM(wxID_FLOPPY, "&Дискета", "Дискета");
		STOCKITEM(wxID_FORWARD, "&Вперёд", "Вперёд");
		STOCKITEM(wxID_HARDDISK, "&Жёсткий диск", "Жёсткий диск");
		STOCKITEM(wxID_HELP, "&Справка", "Справка");
		STOCKITEM(wxID_HOME, "&Домой", "Домой");
		STOCKITEM(wxID_INDENT, "Отступ", "Отступ");
		STOCKITEM(wxID_INDEX, "&Индекс", "Индекс");
		STOCKITEM(wxID_INFO, "&Информация", "Информация");
		STOCKITEM(wxID_ITALIC, "&Курсив", "Курсив");
		STOCKITEM(wxID_JUMP_TO, "&Перейти к", "Перейти к");
		STOCKITEM(wxID_JUSTIFY_CENTER, "По центру", "По центру");
		STOCKITEM(wxID_JUSTIFY_FILL, "По ширине", "По ширине");
		STOCKITEM(wxID_JUSTIFY_LEFT, "По левому краю", "По левому краю");
		STOCKITEM(wxID_JUSTIFY_RIGHT, "По правому краю", "По правому краю");
		STOCKITEM(wxID_LAST, "&Последний", "Последний");
		STOCKITEM(wxID_NETWORK, "&Сеть", "Сеть");
		STOCKITEM(wxID_NEW, "&Новый", "Новый");
		STOCKITEM(wxID_NO, "&Нет", "Нет");
		STOCKITEM(wxID_OK, "&OK", "OK");
		STOCKITEM(wxID_OPEN, "&Открыть...", "Открыть...");
		STOCKITEM(wxID_PASTE, "&Вставить", "Вставить");
		STOCKITEM(wxID_PREFERENCES, "&Предпочтения", "Предпочтения");
		STOCKITEM(wxID_PREVIEW, "Просмотр перед пе&чатью...", "Просмотр перед печатью...");
		STOCKITEM(wxID_PRINT, "&Печать...", "Печать...");
		STOCKITEM(wxID_PROPERTIES, "&Свойства", "Свойства");
		STOCKITEM(wxID_REDO, "&Вернуть", "Вернуть");
		STOCKITEM(wxID_REFRESH, "Обновить", "Обновить");
		STOCKITEM(wxID_REMOVE, "Удалить", "Удалить");
		STOCKITEM(wxID_REPLACE, "За&менить", "Заменить");
		STOCKITEM(wxID_REVERT_TO_SAVED, "Вернуть к сохранённому состоянию", "Вернуть к сохранённому состоянию");
		STOCKITEM(wxID_SAVE, "&Сохранить", "Сохранить");
		STOCKITEM(wxID_SAVEAS, "&Сохранить как", "Сохранить как");
		STOCKITEM(wxID_SELECTALL, "Выбрать &всё", "Выбрать всё");
		STOCKITEM(wxID_SELECT_COLOR, "&Цвет", "Цвет");
		STOCKITEM(wxID_SELECT_FONT, "&Шрифт", "Шрифт");
		STOCKITEM(wxID_SORT_ASCENDING, "&По возрастанию", "По возрастанию");
		STOCKITEM(wxID_SORT_DESCENDING, "По &убыванию", "По убыванию");
		STOCKITEM(wxID_SPELL_CHECK, "&Проверка орфографии", "Проверка орфографии");
		STOCKITEM(wxID_STOP, "&Стоп", "Стоп");
		STOCKITEM(wxID_STRIKETHROUGH, "&Зачёркнутый", "Зачёркнутый");
		STOCKITEM(wxID_TOP, "&Верх", "Верх");
		STOCKITEM(wxID_UNDELETE, "Отменть удаление", "Отменть удаление");
		STOCKITEM(wxID_UNDERLINE, "&Подчёркнутый", "Подчёркнутый");
		STOCKITEM(wxID_UNDO, "&Отменить", "Отменить");
		STOCKITEM(wxID_UNINDENT, "&Убрать отступ", "Убрать отступ");
		STOCKITEM(wxID_UP, "&Вверх", "Вверх");
		STOCKITEM(wxID_YES, "&Да", "Да");
		STOCKITEM(wxID_ZOOM_100, "&Фактический размер", "Фактический размер");
		STOCKITEM(wxID_ZOOM_FIT, "Подогнать по &размеру", "Подогнать по размеру");
		STOCKITEM(wxID_ZOOM_IN, "Уве&личить масштаб", "Увеличить масштаб");
		STOCKITEM(wxID_ZOOM_OUT, "Уме&ньшить масштаб", "Уменьшить масштаб");

		default:
			wxFAIL_MSG(wxT("invalid stock item ID"));
			break;
	};
	*/

	if (flags & wxSTOCK_WITHOUT_ELLIPSIS)
	{
		wxString sBaseLabel;
		if (sStockLabel.EndsWith("...", &sBaseLabel))
		{
			sStockLabel = sBaseLabel;
		}

		// accelerators only make sense for the menu items which should have
		// ellipsis too while wxSTOCK_WITHOUT_ELLIPSIS is mostly useful for
		// buttons which shouldn't have accelerators in their labels
	}

	#if wxUSE_ACCEL
	if (!sStockLabel.empty() && (flags & wxSTOCK_WITH_ACCELERATOR))
	{
		wxAcceleratorEntry tAccel = wxGetStockAccelerator(id);
		if (tAccel.IsOk())
		{
			sStockLabel << wxT('\t') << tAccel.ToString();
		}
	}
	#endif // wxUSE_ACCEL

	return sStockLabel;
}
wxString wxGetStockHelpStringLocalized(wxWindowID id, wxStockHelpStringClient client)
{
	wxString sStockHelp;

	#define STOCKITEM(stockid, ctx, sDefaultLabel)								\
        case wx##stockid:														\
		{																		\
			if (client == ctx)													\
			{																	\
				sStockHelp = wxGetStockLabelAux(#stockid##"_LONG", 0, sDefaultLabel);	\
			}																	\
			break;																\
		}

	switch (id)
	{
		// NB: these help string should be not too specific as they could be used in completely different programs!
		STOCKITEM(ID_ABOUT, wxSTOCK_MENU, u8"Show about dialog")
		STOCKITEM(ID_COPY, wxSTOCK_MENU, u8"Copy selection")
		STOCKITEM(ID_CUT, wxSTOCK_MENU, u8"Cut selection")
		STOCKITEM(ID_DELETE, wxSTOCK_MENU, u8"Delete selection")
		STOCKITEM(ID_REPLACE, wxSTOCK_MENU, u8"Replace selection")
		STOCKITEM(ID_PASTE, wxSTOCK_MENU, u8"Paste selection")
		STOCKITEM(ID_EXIT, wxSTOCK_MENU, u8"Quit this program")
		STOCKITEM(ID_UNDO, wxSTOCK_MENU, u8"Undo last action")
		STOCKITEM(ID_REDO, wxSTOCK_MENU, u8"Redo last action")
		STOCKITEM(ID_CLOSE, wxSTOCK_MENU, u8"Close current document")
		STOCKITEM(ID_SAVE, wxSTOCK_MENU, u8"Save current document")
		STOCKITEM(ID_SAVEAS, wxSTOCK_MENU, u8"Save current document with a different filename")

		default:
		{
			// there's no stock help string for this ID / client
			return wxEmptyString;
		}
	};
	/*
	switch (id)
	{
		// NB: these help string should be not too specific as they could be used in completely different programs!
		STOCKITEM(wxID_ABOUT, wxSTOCK_MENU, _("Показать информацию о программе"))
		STOCKITEM(wxID_COPY, wxSTOCK_MENU, _("Копировать выделенное"))
		STOCKITEM(wxID_CUT, wxSTOCK_MENU, _("Вырезать выделенное"))
		STOCKITEM(wxID_DELETE, wxSTOCK_MENU, _("Удалить выделенное"))
		STOCKITEM(wxID_REPLACE, wxSTOCK_MENU, _("Заменить выделенное"))
		STOCKITEM(wxID_PASTE, wxSTOCK_MENU, _("Вставить выделенное"))
		STOCKITEM(wxID_EXIT, wxSTOCK_MENU, _("Выйти из программы"))
		STOCKITEM(wxID_REDO, wxSTOCK_MENU, _("Вернуть последнее действие"))
		STOCKITEM(wxID_UNDO, wxSTOCK_MENU, _("Отменить последнее действие"))
		STOCKITEM(wxID_CLOSE, wxSTOCK_MENU, _("Закрыть текущий документ"))
		STOCKITEM(wxID_SAVE, wxSTOCK_MENU, _("Сохранить текущий документ"))
		STOCKITEM(wxID_SAVEAS, wxSTOCK_MENU, _("Сохранить текущий документ под другим именем"))
		default:
		{
			// there's no stock help string for this ID / client
			return wxEmptyString;
		}
	};
	*/
	#undef STOCKITEM

	return sStockHelp;
}
