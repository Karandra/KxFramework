#include "KxStdAfx.h"
#include "KxFramework/KxShell.h"
#include "KxFramework/KxSystem.h"
#include "KxFramework/KxDrive.h"
#include "KxFramework/KxFile.h"
#include "KxFramework/KxUtility.h"
#include <KnownFolders.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <Shldisp.h>

HWND KxShell::GetOwnerHWND(wxWindow* window)
{
	window = wxGetTopLevelParent(window);

	HWND hWnd = NULL;
	if (window)
	{
		hWnd = window->GetHandle();
	}
	else
	{
		window = wxTheApp->GetTopWindow();
		if (window)
		{
			hWnd = window->GetHandle();
		}
		else
		{
			window = wxGetTopLevelParent(wxGetActiveWindow());
			if (window)
			{
				return window->GetHandle();
			}
		}
	}

	return hWnd ? hWnd : ::GetShellWindow();
}

KxShellFolderID KxShell::GetShellFolderID(const wxString& name)
{
	KxShellFolderID id = KxSHF_MAX_ID;
	auto it = std::find_if(ms_KnownFoldersTable.begin(), ms_KnownFoldersTable.end(), [name](const KnownFoldersList::value_type& tValue)
	{
		return name == tValue.second.second;
	});

	if (it != ms_KnownFoldersTable.cend())
	{
		return it->first;
	}
	return id;
}
wxString KxShell::GetFolder(KxShellFolderID id, bool useDefault, bool create)
{
	DWORD flags = KF_FLAG_DONT_VERIFY;
	if (useDefault)
	{
		flags |= KF_FLAG_DEFAULT_PATH;
	}
	if (create)
	{
		flags |= KF_FLAG_CREATE|KF_FLAG_INIT;
	}

	wxString out;
	bool isEnvUsed = false;
	bool isCustomMethodUsed = false;

	// In non 64-bit systems this function will retrieve 32-bit versions of this paths.
	if (!KxSystem::Is64Bit())
	{
		switch (id)
		{
			case KxSHF_PROGRAMFILES_X64:
			{
				id = KxSHF_PROGRAMFILES;
				break;
			}
			case KxSHF_COMMONFILES_X64:
			{
				id = KxSHF_COMMONFILES;
				break;
			}
		};
	}
	else
	{
		wxString envVariable;
		switch (id)
		{
			case KxSHF_PROGRAMFILES_X64:
			{
				isEnvUsed = true;
				envVariable = "%ProgramW6432%";
				break;
			}
			case KxSHF_COMMONFILES_X64:
			{
				isEnvUsed = true;
				envVariable = "%CommonProgramW6432%";
				break;
			}
		}

		if (isEnvUsed)
		{
			DWORD size = ExpandEnvironmentStringsW(envVariable, NULL, NULL);
			if (ExpandEnvironmentStringsW(envVariable, wxStringBuffer(out, size), size) == 0)
			{
				out = wxEmptyString;
			}
		}
	}

	if (!isEnvUsed)
	{
		switch (id)
		{
			case KxSHF_TEMP:
			{
				GetTempPathW(INT16_MAX, wxStringBuffer(out, INT16_MAX));
				if (!out.IsEmpty())
				{
					out.RemoveLast(1);
				}

				isCustomMethodUsed = true;
				break;
			}
			case KxSHF_SYSTEMDRIVE:
			{
				GetSystemDirectoryW(wxStringBuffer(out, INT16_MAX), INT16_MAX);
				out = out.Left(2);

				isCustomMethodUsed = true;
				break;
			}
		}
	}

	if (!isEnvUsed && !isCustomMethodUsed)
	{
		if (ms_KnownFoldersTable.count(id))
		{
			PWSTR sPathOut = NULL;
			::SHGetKnownFolderPath(ms_KnownFoldersTable.at(id).first, flags, NULL, &sPathOut);
			if (sPathOut)
			{
				out = sPathOut;
				CoTaskMemFree(sPathOut);
			}
		}
		else
		{
			return out;
		}
	}

	if ((isEnvUsed || isCustomMethodUsed) && create)
	{
		KxFile(out).CreateFolder();
	}
	return out;
}

bool KxShell::FileOperation(const wxString& fullPath, KxFileSearchType elementType, KxShellOperationFunc func, bool useRecycleBin, bool recurse, wxWindow* window)
{
	return FileOperationEx(func, fullPath, wxEmptyString, window, recurse, elementType == KxFS_FILE, useRecycleBin);
}
bool KxShell::FileOperationEx(KxShellOperationFunc func, const wxString& from, const wxString& to, wxWindow* window, bool recurse, bool filesOnly, bool allowUndo, bool yesToAll)
{
	auto CreateZZString = [](const wxString& s)
	{
		if (!s.IsEmpty())
		{
			wxMemoryBuffer stringZZ(s.Length() * sizeof(wxChar) + 2 * sizeof(wxChar));
			stringZZ.AppendData(s.wx_str(), s.Length() * sizeof(wxChar));
			stringZZ.AppendData(L"\000", sizeof(wxChar));
			stringZZ.AppendData(L"\000", sizeof(wxChar));

			return stringZZ;
		}
		return wxMemoryBuffer(0);
	};
	
	wxMemoryBuffer fromZZ = CreateZZString(from);
	wxMemoryBuffer toZZ = CreateZZString(to);

	// Parent window
	window = wxGetTopLevelParent(window);

	// Fill the struct
	SHFILEOPSTRUCTW operationInfo = {0};
	operationInfo.hwnd = window ? window->GetHandle() : NULL;
	operationInfo.wFunc = func;
	operationInfo.pFrom = (PCZZWSTR)fromZZ.GetData();
	operationInfo.pTo = (PCZZWSTR)toZZ.GetData();
	operationInfo.fFlags = FOF_NOCONFIRMMKDIR;
	if (!window)
	{
		operationInfo.fFlags |= FOF_NO_UI;
	}
	if (filesOnly)
	{
		operationInfo.fFlags |= FOF_FILESONLY|FOF_NORECURSION;
	}
	if (allowUndo)
	{
		operationInfo.fFlags |= FOF_ALLOWUNDO;
	}
	if (yesToAll)
	{
		operationInfo.fFlags |= FOF_NOCONFIRMATION;
	}
	if (!recurse)
	{
		operationInfo.fFlags |= FOF_NORECURSION;
	}

	// Disable parent window if UI actions is allowed
	bool disabled = false;
	if (window && !window->IsEnabled())
	{
		window->Disable();
		disabled = true;
	}

	int returnCode = ::SHFileOperationW(&operationInfo);

	// Re-enable parent window
	if (disabled)
	{
		window->Enable();
	}

	// Zero means function succeed.
	return returnCode == 0;
}

DWORD KxShell::FormatDrive(wxWindow* window, const KxDrive& drive, DWORD options, DWORD formatID)
{
	if (drive.IsOK())
	{
		return ::SHFormatDrive(GetOwnerHWND(window), (UINT)drive.ToIndex(), formatID, options);
	}
	return SHFMT_ERROR;
}
bool KxShell::PinShortcut(const wxString& shortcutPath, KxShellShortcutPinMode mode)
{
	int ret = 0;
	switch (mode)
	{
		case KxSH_PIN_STARTMENU:
		{
			#pragma warning(suppress: 4311)
			#pragma warning(suppress: 4302)
			ret = (int)::ShellExecuteW(NULL, L"startpin", shortcutPath, NULL, NULL, SW_SHOWNORMAL);
			break;
		}
		case KxSH_UNPIN_STARTMENU:
		{
			#pragma warning(suppress: 4311)
			#pragma warning(suppress: 4302)
			ret = (int)::ShellExecuteW(NULL, L"startunpin", shortcutPath, NULL, NULL, SW_SHOWNORMAL);
			break;
		}
		case KxSH_PIN_TASKBAR:
		{
			#pragma warning(suppress: 4311)
			#pragma warning(suppress: 4302)
			ret = (int)::ShellExecuteW(NULL, L"taskbarpin", shortcutPath, NULL, NULL, SW_SHOWNORMAL);
			break;
		}
		case KxSH_UNPIN_TASKBAR:
		{
			#pragma warning(suppress: 4311)
			#pragma warning(suppress: 4302)
			ret = (int)::ShellExecuteW(NULL, L"taskbarunpin", shortcutPath, NULL, NULL, SW_SHOWNORMAL);
			break;
		}
	}
	return ret > 32;
}

wxString KxShell::QueryAssocString(const wxString& string, KxShellAssocQuery infoType)
{
	wxString extWithDot = string.AfterLast('.');
	if (!extWithDot.IsEmpty() && extWithDot[0] != '.')
	{
		extWithDot.Prepend(".");
	}

	DWORD length = 0;
	ASSOCF flags = ASSOCF_INIT_DEFAULTTOSTAR;
	::AssocQueryStringW(flags, (ASSOCSTR)infoType, extWithDot, NULL, NULL, &length);

	wxString out;
	if (length != 0)
	{
		::AssocQueryStringW(flags, (ASSOCSTR)infoType, extWithDot, NULL, wxStringBuffer(out, length), &length);
	}
	return out;
}
wxString KxShell::GetTypeIcon(const wxString& ext, int* indexOut)
{
	wxString icon = QueryAssocString(ext, KxSH_AQ_DEFAULT_ICON);
	if (icon == "%1")
	{
		icon = wxEmptyString;
	}

	if (indexOut)
	{
		wxString index = icon.AfterLast(',');

		long value = 0;
		index.ToCLong(&value);
		*indexOut = value;
	}

	wxString path = icon.BeforeLast(',');
	return path.IsEmpty() ? icon : path;
}
wxString KxShell::GetLocalizedName(const wxString& objectPath, int* resourceIDOut)
{
	wxString out;
	int resourceID = 0;
	if (::SHGetLocalizedName(objectPath, wxStringBuffer(out, INT16_MAX), INT16_MAX, &resourceID) == S_OK)
	{
		KxUtility::SetIfNotNull(resourceIDOut, resourceID);
		return out;
	}
	return wxEmptyString;
}

bool KxShell::Execute(wxWindow* window,
					  const wxString& filePath,
					  const wxString& operation,
					  const wxString& arguments,
					  const wxString& workingFolder,
					  int windowMode,
					  bool hideUI
)
{
	SHELLEXECUTEINFOW info = {NULL};
	info.cbSize = sizeof(info);
	info.fMask = SEE_MASK_DEFAULT|SEE_MASK_INVOKEIDLIST;
	if (hideUI)
	{
		info.fMask = info.fMask|SEE_MASK_FLAG_NO_UI;
	}
	info.hwnd = GetOwnerHWND(window);
	info.lpVerb = operation;
	info.lpFile = filePath;
	info.lpDirectory = workingFolder.IsEmpty() ? NULL : workingFolder.wc_str();
	info.lpParameters = arguments.IsEmpty() ? NULL : arguments.wc_str();
	info.nShow = windowMode;

	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED|COINIT_DISABLE_OLE1DDE);
	BOOL status = ::ShellExecuteExW(&info);
	::CoUninitialize();
	return status;
}
bool KxShell::OpenFolderAndSelectItem(const wxString& filePath)
{
	LPITEMIDLIST item = NULL;
	SFGAOF attributes = 0;
	if (SUCCEEDED(::SHParseDisplayName(filePath, NULL, &item, 0, &attributes)))
	{
		::CoInitialize(NULL);
		HRESULT status = ::SHOpenFolderAndSelectItems(item, 0, NULL, 0);
		::CoTaskMemFree(item);
		::CoUninitialize();

		return status == S_OK;
	}
	return false;
}
