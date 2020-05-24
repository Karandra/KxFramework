#pragma once
#include "../Common.h"
#include "Kx/General/IndexedEnum.h"
#include "Kx/System/KnownDirectoryID.h"

namespace kxf::Shell::Private
{
	#define KSHDItem(id)	{KnownDirectoryID::##id, wxS(#id)}
	struct KnownDirectoryDefinition final: IndexedEnumDefinition<KnownDirectoryDefinition, KnownDirectoryID, StringView, true>
	{
		static inline constexpr TItem Items[] =
		{
			KSHDItem(Music),
			KSHDItem(MusicCommon),
			KSHDItem(MusicLibrary),
			KSHDItem(MusicSample),
			KSHDItem(MusicPlaylists),
			KSHDItem(MusicSamplePlaylists),

			KSHDItem(Pictures),
			KSHDItem(PicturesCommon),
			KSHDItem(PicturesLibrary),
			KSHDItem(PicturesPhotoalbums),
			KSHDItem(PicturesSkydrive),
			KSHDItem(PicturesScreenshots),
			KSHDItem(PicturesSampleCommon),
			KSHDItem(PicturesAccounts),
			KSHDItem(PicturesAccountsCommon),
			KSHDItem(PicturesSaved),
			KSHDItem(PicturesSavedLibrary),
			KSHDItem(PicturesCameraRoll),
			KSHDItem(PicturesCameraRollOneDrive),

			KSHDItem(Videos),
			KSHDItem(VideosCommon),
			KSHDItem(VideosLibrary),
			KSHDItem(VideosSample),

			KSHDItem(Desktop),
			KSHDItem(DesktopCommon),

			KSHDItem(Documents),
			KSHDItem(DocumentsCommon),
			KSHDItem(DocumentsLibrary),
			KSHDItem(DocumentsOneDrive),

			KSHDItem(StartMenu),
			KSHDItem(StartMenuCommon),

			KSHDItem(StartMenuPrograms),
			KSHDItem(StartMenuProgramsCommon),

			KSHDItem(Startup),
			KSHDItem(StartupCommon),

			KSHDItem(CommonFiles),
			KSHDItem(CommonFilesX86),
			KSHDItem(CommonFilesX64),

			KSHDItem(ProgramFiles),
			KSHDItem(ProgramFilesX86),
			KSHDItem(ProgramFilesX64),

			KSHDItem(SystemDrive),
			KSHDItem(Windows),
			KSHDItem(Fonts),
			KSHDItem(Temp),

			KSHDItem(System),
			KSHDItem(SystemX86),

			KSHDItem(ApplicationData),
			KSHDItem(ApplicationDataLocal),
			KSHDItem(ApplicationDataLocalLow),

			KSHDItem(AdminTools),
			KSHDItem(AdminToolsCommon),
			KSHDItem(ApplicationShortcuts),
			KSHDItem(CDBurning),
			KSHDItem(OEMLinksCommon),
			KSHDItem(Templates),
			KSHDItem(TemplatesCommon),
			KSHDItem(Contacts),
			KSHDItem(Cookies),
			KSHDItem(DeviceMetaDataStore),
			KSHDItem(Downloads),
			KSHDItem(Favorites),
			KSHDItem(GameTasks),
			KSHDItem(GameTasksCommon),
			KSHDItem(History),
			KSHDItem(ImplicitAppShortcuts),
			KSHDItem(InternetCache),
			KSHDItem(Libraries),
			KSHDItem(LibrariesCommon),
			KSHDItem(Links),
			KSHDItem(LocalizedResources),
			KSHDItem(NetHood),
			KSHDItem(OriginalImages),
			KSHDItem(PrintHood),
			KSHDItem(UserProfile),
			KSHDItem(ProgramData),
			KSHDItem(ProgramDataCommon),
			KSHDItem(Ringtones),
			KSHDItem(RingtonesCommon),
			KSHDItem(QuickLaunch),
			KSHDItem(Recent),
			KSHDItem(RecordedtvLibrary),
			KSHDItem(ResourceDir),
			KSHDItem(RoamedTileImages),
			KSHDItem(RoamingTiles),
			KSHDItem(SavedGames),
			KSHDItem(SearchHistory),
			KSHDItem(SearchTemplates),
			KSHDItem(SendTo),
			KSHDItem(SidebarParts),
			KSHDItem(SidebarPartsDefault),
			KSHDItem(OneDrive),
			KSHDItem(UserPinned),
			KSHDItem(UserProfiles),
			KSHDItem(UserProgramFiles),
			KSHDItem(UserProgramFilesCommon)
		};
	};
	#undef KSHDItem
}
