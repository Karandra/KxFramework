#include "KxStdAfx.h"
#include "KxFramework/KxShell.h"

#define AddRecord(ID, GUID)		std::make_pair(Kx##ID, std::make_pair(GUID, #ID))

const KxShell::KnownFoldersList KxShell::ms_KnownFoldersTable =
{
	// Elements
	AddRecord(SHF_MUSIC, FOLDERID_Music),
	AddRecord(SHF_MUSIC_COMMON, FOLDERID_PublicMusic),
	AddRecord(SHF_MUSIC_LIBRARY, FOLDERID_MusicLibrary),
	AddRecord(SHF_MUSIC_SAMPLE, FOLDERID_SampleMusic),
	AddRecord(SHF_MUSIC_PLAYLISTS, FOLDERID_Playlists),
	AddRecord(SHF_MUSIC_SAMPLE_PLAYLISTS, FOLDERID_SamplePlaylists),

	AddRecord(SHF_PICTURES, FOLDERID_Pictures),
	AddRecord(SHF_PICTURES_COMMON, FOLDERID_PublicPictures),
	AddRecord(SHF_PICTURES_LIBRARY, FOLDERID_PicturesLibrary),
	AddRecord(SHF_PICTURES_PHOTOALBUMS, FOLDERID_PhotoAlbums),
	AddRecord(SHF_PICTURES_SKYDRIVE, FOLDERID_SkyDrivePictures),
	AddRecord(SHF_PICTURES_SCREENSHOTS, FOLDERID_Screenshots),
	AddRecord(SHF_PICTURES_SAMPLE_COMMON, FOLDERID_SamplePictures),
	AddRecord(SHF_PICTURES_ACCOUNTS, FOLDERID_AccountPictures),
	AddRecord(SHF_PICTURES_ACCOUNTS_COMMON, FOLDERID_PublicUserTiles),
	AddRecord(SHF_PICTURES_SAVED, FOLDERID_SavedPictures),
	AddRecord(SHF_PICTURES_SAVED_LIBRARY, FOLDERID_SavedPicturesLibrary),
	AddRecord(SHF_PICTURES_CAMERAROLL, FOLDERID_CameraRoll),
	AddRecord(SHF_PICTURES_CAMERAROLL_SKYDRIVE, FOLDERID_SkyDriveCameraRoll),

	AddRecord(SHF_VIDEOS, FOLDERID_Videos),
	AddRecord(SHF_VIDEOS_COMMON, FOLDERID_PublicVideos),
	AddRecord(SHF_VIDEOS_LIBRARY, FOLDERID_VideosLibrary),
	AddRecord(SHF_VIDEOS_SAMPLE, FOLDERID_SampleVideos),

	AddRecord(SHF_PICTURES_CAMERAROLL_SKYDRIVE, FOLDERID_Desktop),
	AddRecord(SHF_PICTURES_CAMERAROLL_SKYDRIVE, FOLDERID_PublicDesktop),

	AddRecord(SHF_DESKTOP, FOLDERID_Desktop),
	AddRecord(SHF_DESKTOP_COMMON, FOLDERID_PublicDesktop),

	AddRecord(SHF_DOCUMENTS, FOLDERID_Documents),
	AddRecord(SHF_DOCUMENTS_COMMON, FOLDERID_PublicDocuments),
	AddRecord(SHF_DOCUMENTS_LIBRARY, FOLDERID_DocumentsLibrary),
	AddRecord(SHF_DOCUMENTS_SKYDRIVE, FOLDERID_SkyDriveDocuments),

	AddRecord(SHF_STARTMENU, FOLDERID_StartMenu),
	AddRecord(SHF_STARTMENU_COMMON, FOLDERID_CommonStartMenu),

	AddRecord(SHF_STARTMENUPROGRAMS, FOLDERID_Programs),
	AddRecord(SHF_STARTMENUPROGRAMS_COMMON, FOLDERID_CommonPrograms),

	AddRecord(SHF_STARTUP, FOLDERID_Startup),
	AddRecord(SHF_STARTUP_COMMON, FOLDERID_CommonStartup),

	AddRecord(SHF_COMMONFILES, FOLDERID_ProgramFilesCommon),
	AddRecord(SHF_COMMONFILES_X86, FOLDERID_ProgramFilesCommonX86),
	AddRecord(SHF_COMMONFILES_X64, FOLDERID_ProgramFilesCommonX64),

	AddRecord(SHF_PROGRAMFILES, FOLDERID_ProgramFiles),
	AddRecord(SHF_PROGRAMFILES_X86, FOLDERID_ProgramFilesX86),
	AddRecord(SHF_PROGRAMFILES_X64, FOLDERID_ProgramFilesX64),

	AddRecord(SHF_WINDOWS, FOLDERID_Windows),
	AddRecord(SHF_FONTS, FOLDERID_Fonts),
	AddRecord(SHF_TEMP, KxFOLDERID_NULL),
	AddRecord(SHF_SYSTEMDRIVE, KxFOLDERID_NULL),

	AddRecord(SHF_SYSTEM, FOLDERID_System),
	AddRecord(SHF_SYSTEM_X86, FOLDERID_SystemX86),

	AddRecord(SHF_APPLICATIONDATA, FOLDERID_RoamingAppData),
	AddRecord(SHF_APPLICATIONDATA_LOCAL, FOLDERID_LocalAppData),
	AddRecord(SHF_APPLICATIONDATA_LOCAL_LOW, FOLDERID_LocalAppDataLow),

	AddRecord(SHF_ADMINTOOLS, FOLDERID_AdminTools),
	AddRecord(SHF_ADMINTOOLS_COMMON, FOLDERID_CommonAdminTools),
	AddRecord(SHF_APPLICATION_SHORTCUTS, FOLDERID_ApplicationShortcuts),
	AddRecord(SHF_CD_BURNING, FOLDERID_CDBurning),
	AddRecord(SHF_OEMLINKS_COMMON, FOLDERID_CommonOEMLinks),
	AddRecord(SHF_TRMPLATES, FOLDERID_Templates),
	AddRecord(SHF_TRMPLATES_COMMON, FOLDERID_CommonTemplates),
	AddRecord(SHF_CONTACTS, FOLDERID_Contacts),
	AddRecord(SHF_COOKIES, FOLDERID_Cookies),
	AddRecord(SHF_DEVICE_META_DATA_STORE, FOLDERID_DeviceMetadataStore),
	AddRecord(SHF_DOWNLOADS, FOLDERID_Downloads),
	AddRecord(SHF_FAVORITES, FOLDERID_Favorites),
	AddRecord(SHF_GAME_TASKS, FOLDERID_GameTasks),
	AddRecord(SHF_GAME_TASKS_COMMON, FOLDERID_PublicGameTasks),
	AddRecord(SHF_HISTORY, FOLDERID_History),
	AddRecord(SHF_IMPLICIT_APP_SHORTCUTS, FOLDERID_ImplicitAppShortcuts),
	AddRecord(SHF_INTERNET_CACHE, FOLDERID_InternetCache),
	AddRecord(SHF_LIBRARIES, FOLDERID_Libraries),
	AddRecord(SHF_LIBRARIES_COMMON, FOLDERID_PublicLibraries),
	AddRecord(SHF_LINKS, FOLDERID_Links),
	AddRecord(SHF_LOCALIZED_RESOURCES, FOLDERID_LocalizedResourcesDir),
	AddRecord(SHF_NET_HOOD, FOLDERID_NetHood),
	AddRecord(SHF_ORIGINAL_IMAGES, FOLDERID_OriginalImages),
	AddRecord(SHF_PRINT_HOOD, FOLDERID_PrintHood),
	AddRecord(SHF_USER_PROFILE, FOLDERID_Profile),
	AddRecord(SHF_PROGRAM_DATA, FOLDERID_ProgramData),
	AddRecord(SHF_COMMON, FOLDERID_Public),
	AddRecord(SHF_RINGTONES, FOLDERID_Ringtones),
	AddRecord(SHF_RINGTONES_COMMON, FOLDERID_PublicRingtones),
	AddRecord(SHF_QUICK_LAUNCH, FOLDERID_QuickLaunch),
	AddRecord(SHF_RECENT, FOLDERID_Recent),
	AddRecord(SHF_RECORDEDTV_LIBRARY, FOLDERID_RecordedTVLibrary),
	AddRecord(SHF_RESOURCEDIR, FOLDERID_ResourceDir),
	AddRecord(SHF_ROAMED_TILE_IMAGES, FOLDERID_RoamedTileImages),
	AddRecord(SHF_ROAMINGTILES, FOLDERID_RoamingTiles),
	AddRecord(SHF_SAVED_GAMES, FOLDERID_SavedGames),
	AddRecord(SHF_SEARCH_HISTORY, FOLDERID_SearchHistory),
	AddRecord(SHF_SEARCHTEMPLATES, FOLDERID_SearchTemplates),
	AddRecord(SHF_SEND_TO, FOLDERID_SendTo),
	AddRecord(SHF_SIDEBAR_PARTS, FOLDERID_SidebarParts),
	AddRecord(SHF_SIDEBAR_PARTS_DEFAULT, FOLDERID_SidebarDefaultParts),
	AddRecord(SHF_SKYDRIVE, FOLDERID_SkyDrive),
	AddRecord(SHF_USER_PINNED, FOLDERID_UserPinned),
	AddRecord(SHF_USER_PROFILES, FOLDERID_UserProfiles),
	AddRecord(SHF_USER_PROGRAM_FILES, FOLDERID_UserProgramFiles),
	AddRecord(SHF_USER_PROGRAM_FILES_COMMON, FOLDERID_UserProgramFilesCommon),
};
