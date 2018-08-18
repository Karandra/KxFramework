#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxDrive.h"
#include <SHLOBJ.h>
#include <SHLWAPI.h>

#define KxShellDefineKnownFolder(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)		const GUID name = {l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 }}

KxShellDefineKnownFolder(KxFOLDERID_NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

enum KxFileSearchType;
enum KxShellFolderID;
enum KxShellShortcutPinMode
{
	KxSH_PIN_STARTMENU,
	KxSH_UNPIN_STARTMENU,
	KxSH_PIN_TASKBAR,
	KxSH_UNPIN_TASKBAR
};
enum KxShellAssocQuery
{
	KxSH_AQ_COMMAND = ASSOCSTR_COMMAND,
	KxSH_AQ_EXECUTABLE = ASSOCSTR_EXECUTABLE,
	KxSH_AQ_FRIENDLY_DOC_NAME = ASSOCSTR_FRIENDLYDOCNAME,
	KxSH_AQ_FRIENDLY_APP_NAME = ASSOCSTR_FRIENDLYAPPNAME,
	KxSH_AQ_NO_OPEN = ASSOCSTR_NOOPEN,
	KxSH_AQ_SHELL_NEW_VALUE = ASSOCSTR_SHELLNEWVALUE,
	KxSH_AQ_DDE_COMMAND = ASSOCSTR_DDECOMMAND,
	KxSH_AQ_DDE_IF_EXEC = ASSOCSTR_DDEIFEXEC,
	KxSH_AQ_DDE_APPLICATION = ASSOCSTR_DDEAPPLICATION,
	KxSH_AQ_DDE_TOPIC = ASSOCSTR_DDETOPIC,
	KxSH_AQ_INFO_TIP = ASSOCSTR_INFOTIP,
	KxSH_AQ_QUICK_TIP = ASSOCSTR_QUICKTIP,
	KxSH_AQ_TILE_INFO = ASSOCSTR_TILEINFO,
	KxSH_AQ_CONTENT_TYPE = ASSOCSTR_CONTENTTYPE,
	KxSH_AQ_DEFAULT_ICON = ASSOCSTR_DEFAULTICON,
	KxSH_AQ_SHELL_EXTENSION = ASSOCSTR_SHELLEXTENSION,
	KxSH_AQ_SUPPORTED_URI_PROTOCOLS = ASSOCSTR_SUPPORTED_URI_PROTOCOLS,
	
	#if (NTDDI_VERSION >= NTDDI_WIN10)
	KxSH_AQ_DROPTARGET = ASSOCSTR_DROPTARGET,
	KxSH_AQ_DELEGATEEXECUTE = ASSOCSTR_DELEGATEEXECUTE,
	KxSH_AQ_PROGID = ASSOCSTR_PROGID,
	KxSH_AQ_APPID = ASSOCSTR_APPID,
	KxSH_AQ_APPPUBLISHER = ASSOCSTR_APPPUBLISHER,
	KxSH_AQ_APPICONREFERENCE = ASSOCSTR_APPICONREFERENCE,
	#endif

	KxSH_AQ_MAX = ASSOCSTR_MAX,
};
enum KxShellOperationFunc
{
	KxFOF_COPY = FO_COPY,
	KxFOF_MOVE = FO_MOVE,
	KxFOF_RENAME = FO_RENAME,
	KxFOF_DELETE = FO_DELETE,
};

class KxShell
{
	friend class KxShellExecuteThread;

	private:
		using KnownFolderItem = std::pair<GUID, const char*>;
		using KnownFoldersList = std::unordered_map<KxShellFolderID, KnownFolderItem>;
		static const KnownFoldersList ms_KnownFoldersTable;
		
		// Get any valid HWND for functions that requires parent window in all cases.
		// Return HWND of windows in following chain:
		// window -> wxTheApp->GetTopWindow() -> wxGetTopLevelParent(wxGetActiveWindow()) -> ::GetShellWindow()
		static HWND GetOwnerHWND(wxWindow* window = NULL);
		
	public:
		static KxShellFolderID GetShellFolderID(const wxString& name);
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

enum KxShellFolderID: int
{
	KxSHF_MUSIC,
	KxSHF_MUSIC_COMMON,
	KxSHF_MUSIC_LIBRARY,
	KxSHF_MUSIC_SAMPLE,
	KxSHF_MUSIC_PLAYLISTS,
	KxSHF_MUSIC_SAMPLE_PLAYLISTS,

	KxSHF_PICTURES,
	KxSHF_PICTURES_COMMON,
	KxSHF_PICTURES_LIBRARY,
	KxSHF_PICTURES_PHOTOALBUMS,
	KxSHF_PICTURES_SKYDRIVE,
	KxSHF_PICTURES_SCREENSHOTS,
	KxSHF_PICTURES_SAMPLE_COMMON,
	KxSHF_PICTURES_ACCOUNTS,
	KxSHF_PICTURES_ACCOUNTS_COMMON,
	KxSHF_PICTURES_SAVED,
	KxSHF_PICTURES_SAVED_LIBRARY,
	KxSHF_PICTURES_CAMERAROLL,
	KxSHF_PICTURES_CAMERAROLL_SKYDRIVE,

	KxSHF_VIDEOS,
	KxSHF_VIDEOS_COMMON,
	KxSHF_VIDEOS_LIBRARY,
	KxSHF_VIDEOS_SAMPLE,

	KxSHF_DESKTOP,
	KxSHF_DESKTOP_COMMON,

	KxSHF_DOCUMENTS,
	KxSHF_DOCUMENTS_COMMON,
	KxSHF_DOCUMENTS_LIBRARY,
	KxSHF_DOCUMENTS_SKYDRIVE,

	KxSHF_STARTMENU,
	KxSHF_STARTMENU_COMMON,

	KxSHF_STARTMENUPROGRAMS,
	KxSHF_STARTMENUPROGRAMS_COMMON,

	KxSHF_STARTUP,
	KxSHF_STARTUP_COMMON,

	KxSHF_COMMONFILES,
	KxSHF_COMMONFILES_X86,
	KxSHF_COMMONFILES_X64,

	KxSHF_PROGRAMFILES,
	KxSHF_PROGRAMFILES_X86,
	KxSHF_PROGRAMFILES_X64,

	KxSHF_WINDOWS,
	KxSHF_FONTS,
	KxSHF_TEMP,
	KxSHF_SYSTEMDRIVE,

	KxSHF_SYSTEM,
	KxSHF_SYSTEM_X86,

	KxSHF_APPLICATIONDATA,
	KxSHF_APPLICATIONDATA_LOCAL,
	KxSHF_APPLICATIONDATA_LOCAL_LOW,

	KxSHF_ADMINTOOLS,
	KxSHF_ADMINTOOLS_COMMON,
	KxSHF_APPLICATION_SHORTCUTS,
	KxSHF_CD_BURNING,
	KxSHF_OEMLINKS_COMMON,
	KxSHF_TRMPLATES,
	KxSHF_TRMPLATES_COMMON,
	KxSHF_CONTACTS,
	KxSHF_COOKIES,
	KxSHF_DEVICE_META_DATA_STORE,
	KxSHF_DOWNLOADS,
	KxSHF_FAVORITES,
	KxSHF_GAME_TASKS,
	KxSHF_GAME_TASKS_COMMON,
	KxSHF_HISTORY,
	KxSHF_IMPLICIT_APP_SHORTCUTS,
	KxSHF_INTERNET_CACHE,
	KxSHF_LIBRARIES,
	KxSHF_LIBRARIES_COMMON,
	KxSHF_LINKS,
	KxSHF_LOCALIZED_RESOURCES,
	KxSHF_NET_HOOD,
	KxSHF_ORIGINAL_IMAGES,
	KxSHF_PRINT_HOOD,
	KxSHF_USER_PROFILE,
	KxSHF_PROGRAM_DATA,
	KxSHF_COMMON,
	KxSHF_RINGTONES,
	KxSHF_RINGTONES_COMMON,
	KxSHF_QUICK_LAUNCH,
	KxSHF_RECENT,
	KxSHF_RECORDEDTV_LIBRARY,
	KxSHF_RESOURCEDIR,
	KxSHF_ROAMED_TILE_IMAGES,
	KxSHF_ROAMINGTILES,
	KxSHF_SAVED_GAMES,
	KxSHF_SEARCH_HISTORY,
	KxSHF_SEARCHTEMPLATES,
	KxSHF_SEND_TO,
	KxSHF_SIDEBAR_PARTS,
	KxSHF_SIDEBAR_PARTS_DEFAULT,
	KxSHF_SKYDRIVE,
	KxSHF_USER_PINNED,
	KxSHF_USER_PROFILES,
	KxSHF_USER_PROGRAM_FILES,
	KxSHF_USER_PROGRAM_FILES_COMMON,

	// Max
	KxSHF_MAX_ID,

	// Overrides
	KxSHF_APPLICATIONDATA_COMMON = KxSHF_COMMON,
};
