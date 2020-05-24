#pragma once
#include "../Common.h"
#include "kxf/General/NativeUUID.h"
#include "kxf/System/KnownDirectoryID.h"
#include <Windows.h>
#include <KnownFolders.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::Shell::Private
{
	#define KSHDItem(id, guid)	{KnownDirectoryID::##id, guid}
	struct KnownDirectoryGUID final
	{
		static inline std::pair<KnownDirectoryID, ::_GUID> Items[] =
		{
			KSHDItem(Music, FOLDERID_Music),
			KSHDItem(MusicCommon, FOLDERID_PublicMusic),
			KSHDItem(MusicLibrary, FOLDERID_MusicLibrary),
			KSHDItem(MusicSample, FOLDERID_SampleMusic),
			KSHDItem(MusicPlaylists, FOLDERID_Playlists),
			KSHDItem(MusicSamplePlaylists, FOLDERID_SamplePlaylists),

			KSHDItem(Pictures, FOLDERID_Pictures),
			KSHDItem(PicturesCommon, FOLDERID_PublicPictures),
			KSHDItem(PicturesLibrary, FOLDERID_PicturesLibrary),
			KSHDItem(PicturesPhotoalbums, FOLDERID_PhotoAlbums),
			KSHDItem(PicturesSkydrive, FOLDERID_SkyDrivePictures),
			KSHDItem(PicturesScreenshots, FOLDERID_Screenshots),
			KSHDItem(PicturesSampleCommon, FOLDERID_SamplePictures),
			KSHDItem(PicturesAccounts, FOLDERID_AccountPictures),
			KSHDItem(PicturesAccountsCommon, FOLDERID_PublicUserTiles),
			KSHDItem(PicturesSaved, FOLDERID_SavedPictures),
			KSHDItem(PicturesSavedLibrary, FOLDERID_SavedPicturesLibrary),
			KSHDItem(PicturesCameraRoll, FOLDERID_CameraRoll),
			KSHDItem(PicturesCameraRollOneDrive, FOLDERID_SkyDriveCameraRoll),

			KSHDItem(Videos, FOLDERID_Videos),
			KSHDItem(VideosCommon, FOLDERID_PublicVideos),
			KSHDItem(VideosLibrary, FOLDERID_VideosLibrary),
			KSHDItem(VideosSample, FOLDERID_SampleVideos),

			KSHDItem(Desktop, FOLDERID_Desktop),
			KSHDItem(DesktopCommon, FOLDERID_PublicDesktop),

			KSHDItem(Documents, FOLDERID_Documents),
			KSHDItem(DocumentsCommon, FOLDERID_PublicDocuments),
			KSHDItem(DocumentsLibrary, FOLDERID_DocumentsLibrary),
			KSHDItem(DocumentsOneDrive, FOLDERID_SkyDriveDocuments),

			KSHDItem(StartMenu, FOLDERID_StartMenu),
			KSHDItem(StartMenuCommon, FOLDERID_CommonStartMenu),

			KSHDItem(StartMenuPrograms, FOLDERID_Programs),
			KSHDItem(StartMenuProgramsCommon, FOLDERID_CommonPrograms),

			KSHDItem(Startup, FOLDERID_Startup),
			KSHDItem(StartupCommon, FOLDERID_CommonStartup),

			KSHDItem(CommonFiles, FOLDERID_ProgramFilesCommon),
			KSHDItem(CommonFilesX86, FOLDERID_ProgramFilesCommonX86),
			KSHDItem(CommonFilesX64, FOLDERID_ProgramFilesCommonX64),

			KSHDItem(ProgramFiles, FOLDERID_ProgramFiles),
			KSHDItem(ProgramFilesX86, FOLDERID_ProgramFilesX86),
			KSHDItem(ProgramFilesX64, FOLDERID_ProgramFilesX64),

			KSHDItem(SystemDrive, GUID_NULL),
			KSHDItem(Windows, FOLDERID_Windows),
			KSHDItem(Fonts, FOLDERID_Fonts),
			KSHDItem(Temp, GUID_NULL),

			KSHDItem(System, FOLDERID_System),
			KSHDItem(SystemX86, FOLDERID_SystemX86),

			KSHDItem(ApplicationData, FOLDERID_RoamingAppData),
			KSHDItem(ApplicationDataLocal, FOLDERID_LocalAppData),
			KSHDItem(ApplicationDataLocalLow, FOLDERID_LocalAppDataLow),

			KSHDItem(AdminTools, FOLDERID_AdminTools),
			KSHDItem(AdminToolsCommon, FOLDERID_CommonAdminTools),
			KSHDItem(ApplicationShortcuts, FOLDERID_ApplicationShortcuts),
			KSHDItem(CDBurning, FOLDERID_CDBurning),
			KSHDItem(OEMLinksCommon, FOLDERID_CommonOEMLinks),
			KSHDItem(Templates, FOLDERID_Templates),
			KSHDItem(TemplatesCommon, FOLDERID_CommonTemplates),
			KSHDItem(Contacts, FOLDERID_Contacts),
			KSHDItem(Cookies, FOLDERID_Cookies),
			KSHDItem(DeviceMetaDataStore, FOLDERID_DeviceMetadataStore),
			KSHDItem(Downloads, FOLDERID_Downloads),
			KSHDItem(Favorites, FOLDERID_Favorites),
			KSHDItem(GameTasks, FOLDERID_GameTasks),
			KSHDItem(GameTasksCommon, FOLDERID_PublicGameTasks),
			KSHDItem(History, FOLDERID_History),
			KSHDItem(ImplicitAppShortcuts, FOLDERID_ImplicitAppShortcuts),
			KSHDItem(InternetCache, FOLDERID_InternetCache),
			KSHDItem(Libraries, FOLDERID_Libraries),
			KSHDItem(LibrariesCommon, FOLDERID_PublicLibraries),
			KSHDItem(Links, FOLDERID_Links),
			KSHDItem(LocalizedResources, FOLDERID_LocalizedResourcesDir),
			KSHDItem(NetHood, FOLDERID_NetHood),
			KSHDItem(OriginalImages, FOLDERID_OriginalImages),
			KSHDItem(PrintHood, FOLDERID_PrintHood),
			KSHDItem(UserProfile, FOLDERID_Profile),
			KSHDItem(ProgramData, FOLDERID_ProgramData),
			KSHDItem(ProgramDataCommon, FOLDERID_Public),
			KSHDItem(Ringtones, FOLDERID_Ringtones),
			KSHDItem(RingtonesCommon, FOLDERID_PublicRingtones),
			KSHDItem(QuickLaunch, FOLDERID_QuickLaunch),
			KSHDItem(Recent, FOLDERID_Recent),
			KSHDItem(RecordedtvLibrary, FOLDERID_RecordedTVLibrary),
			KSHDItem(ResourceDir, FOLDERID_ResourceDir),
			KSHDItem(RoamedTileImages, FOLDERID_RoamedTileImages),
			KSHDItem(RoamingTiles, FOLDERID_RoamingTiles),
			KSHDItem(SavedGames, FOLDERID_SavedGames),
			KSHDItem(SearchHistory, FOLDERID_SearchHistory),
			KSHDItem(SearchTemplates, FOLDERID_SearchTemplates),
			KSHDItem(SendTo, FOLDERID_SendTo),
			KSHDItem(SidebarParts, FOLDERID_SidebarParts),
			KSHDItem(SidebarPartsDefault, FOLDERID_SidebarDefaultParts),
			KSHDItem(OneDrive, FOLDERID_SkyDrive),
			KSHDItem(UserPinned, FOLDERID_UserPinned),
			KSHDItem(UserProfiles, FOLDERID_UserProfiles),
			KSHDItem(UserProgramFiles, FOLDERID_UserProgramFiles),
			KSHDItem(UserProgramFilesCommon, FOLDERID_UserProgramFilesCommon)
		};
	};
	#undef KSHDItem
}
