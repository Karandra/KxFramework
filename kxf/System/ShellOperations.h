#pragma once
#include "Common.h"
#include "KnownDirectoryID.h"
#include "kxf/Core/String.h"
#include "kxf/System/HResult.h"
#include "kxf/FileSystem/FSPath.h"
class wxWindow;

namespace kxf
{
	class Any;
	class URI;
	class FileItem;
	class LegacyVolume;
	class UniversallyUniqueID;
	class BitmapImage;
}

namespace kxf
{
	enum class SHOperationType
	{
		Copy,
		Move,
		Rename,
		Delete
	};
	enum class SHOperationFlags: uint32_t
	{
		None = 0,
		Recursive = 1 << 1,
		LimitToFiles = 1 << 2,
		AllowUndo = 1 << 3,
		NoConfirmation = 1 << 4,
	};
	enum class SHPinShortcutCommand
	{
		PinStartMenu,
		UnpinStartMenu,

		PinTaskbar,
		UnpinTaskbar,
	};
	enum class SHQueryAssociation
	{
		None = -1,

		Command,
		Executable,
		FriendlyDocName,
		FriendlyAppName,
		InfoTip,
		QuickTip,
		TileInfo,
		ContentType,
		DefaultIcon,
		ShellExtension,
		SupportedURIProtocols,
		ProgID,
		AppID,
		AppPublisher,
		AppIconReference,
	};
	enum class SHGetKnownDirectoryFlag: uint32_t
	{
		None = 0,

		UseDefaultLocation = 1 << 0,
		CreateIfDoesNotExist = 1 << 1,
	};
	enum class SHGetFileIconFlag: uint32_t
	{
		None = 0,

		Small = 1 << 0,
		Large = 1 << 1,
		ShellSized = 1 << 2,
		Selected = 1 << 3,
		Open = 1 << 4,
		AddOverlays = 1 << 5,
		LinkOverlay = 1 << 6,
	};
	enum class SHExexuteFlag: uint32_t
	{
		None = 0,

		Async = 1 << 0,
		HideUI = 1 << 1,
		InheritConsole = 1 << 2,
	};

	KxFlagSet_Declare(SHOperationFlags);
	KxFlagSet_Declare(SHGetKnownDirectoryFlag);
	KxFlagSet_Declare(SHGetFileIconFlag);
	KxFlagSet_Declare(SHExexuteFlag);
}

namespace kxf::Shell
{
	KX_API bool FileOperation(SHOperationType opType, const FSPath& source, const FSPath& destination, wxWindow* window = nullptr, FlagSet<SHOperationFlags> flags = {});
	KX_API bool FormatVolume(const wxWindow* window, const LegacyVolume& volume, bool quickFormat = false) noexcept;
	KX_API bool PinShortcut(const FSPath& filePath, SHPinShortcutCommand command);

	KX_API bool Execute(const wxWindow* window,
						const FSPath& path,
						const String& command = {},
						const String& parameters = {},
						const FSPath& workingDirectory = {},
						FlagSet<SHWindowCommand> showWindow = SHWindowCommand::Show,
						FlagSet<SHExexuteFlag> flags = {}
	);
	KX_API bool OpenURI(const wxWindow* window, const URI& uri, FlagSet<SHWindowCommand> showWindow = SHWindowCommand::Show, FlagSet<SHExexuteFlag> flags = {});
	KX_API HResult ExploreToItem(const FSPath& path);

	KX_API BitmapImage GetFileIcon(const FSPath& path, FlagSet<SHGetFileIconFlag> flags);
	KX_API BitmapImage GetFileIcon(const FileItem& item, FlagSet<SHGetFileIconFlag> flags);

	KX_API String QueryAssociation(const FSPath& filePath, SHQueryAssociation option, Any* extraData = nullptr);
	KX_API String QueryAssociation(const UniversallyUniqueID& classID, SHQueryAssociation option, Any* extraData = nullptr);

	KX_API String GetLocalizedName(const FSPath& path, int* resourceID = nullptr);
	KX_API HResult SetLocalizedName(const FSPath& path, const String& resourse, int resourceID);

	KX_API FSPath GetKnownDirectory(KnownDirectoryID id, FlagSet<SHGetKnownDirectoryFlag> flags = {});
	KX_API size_t EnumKnownDirectories(std::function<bool(KnownDirectoryID, String)> func);
}
