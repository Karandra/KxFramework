/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxShellDefs.h"
#include "KxFramework/KxDrive.h"
#include "KxFramework/KxFile.h"
#include <SHLOBJ.h>
#include <SHLWAPI.h>
class KX_API KxFileItem;

class KX_API KxShell
{
	friend class KxShellExecuteThread;

	public:
		using KnownFolderItem = std::pair<GUID, const wxChar*>;
		using KnownFoldersList = std::unordered_map<KxShellFolderID, KnownFolderItem>;

	private:
		// Get any valid HWND for functions that requires parent window in all cases.
		// Return HWND of windows in following chain:
		// window -> wxTheApp->GetTopWindow() -> wxGetTopLevelParent(wxGetActiveWindow()) -> ::GetShellWindow()
		static HWND GetOwnerHWND(wxWindow* window = NULL);
		
	public:
		static KxShellFolderID GetShellFolderID(const wxString& name);
		static const KnownFoldersList& GetShellFolderList();
		static wxString GetFolder(KxShellFolderID id, bool useDefault = false, bool create = false);
		
		static bool FileOperation(const wxString& fullPath, KxFileSearchType elementType, KxShellOperationFunc func, bool useRecycleBin = false, bool recurse = false, wxWindow* window = NULL);
		static bool FileOperationEx(KxShellOperationFunc func, const wxString& from, const wxString& to, wxWindow* window = NULL, bool recurse = false, bool filesOnly = true, bool allowUndo = true, bool yesToAll = false);
		static DWORD FormatDrive(wxWindow* window, const KxDrive& drive, DWORD options = SHFMT_OPT_FULL, DWORD formatID = SHFMT_ID_DEFAULT);
		static bool PinShortcut(const wxString& shortcutPath, KxShellShortcutPinMode mode);
		
		static wxString QueryAssocString(const wxString& string, KxShellAssocQuery infoType);
		static wxString GetDefaultViewer(const wxString& ext)
		{
			return QueryAssocString(ext, KxSH_AQ_EXECUTABLE);
		}
		static wxString GetDefaultViewerName(const wxString& ext)
		{
			return QueryAssocString(ext, KxSH_AQ_FRIENDLY_APP_NAME);
		}
		static wxString GetTypeName(const wxString& ext)
		{
			return QueryAssocString(ext, KxSH_AQ_FRIENDLY_DOC_NAME);
		}
		static wxString GetTypeIcon(const wxString& ext, int* indexOut = NULL);
		static wxString GetTypeCommand(const wxString& ext)
		{
			return QueryAssocString(ext, KxSH_AQ_COMMAND);
		}
		static wxString GetLocalizedName(const wxString& objectPath, int* resourceIDOut = NULL);

		static wxIcon GetFileIcon(const wxString& path, bool smallIcon = false);
		static wxIcon GetFileIcon(const KxFileItem& item, bool smallIcon = false);
		
		static bool Execute(wxWindow* window,
							const wxString& filePath,
							const wxString& operation = wxEmptyString,
							const wxString& arguments = wxEmptyString,
							const wxString& workingFolder = wxEmptyString,
							int windowMode = SW_SHOWNORMAL,
							bool hideUI = false
		);
		static bool OpenFolderAndSelectItem(const wxString& filePath);
};
