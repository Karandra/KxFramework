#pragma once
#include "Common.h"
#include "KnownDirectoryID.h"
#include "Kx/General/String.h"
#include "Kx/FileSystem/FSPath.h"
#include "Kx/System/ErrorCodeValue.h"
#include <wx/icon.h>
class wxWindow;

namespace kxf
{
	class Any;
	class URI;
	class FileItem;
	class LegacyVolume;
	class UniversallyUniqueID;
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
	enum class SHOperationFlags
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
	enum class SHGetKnownDirectoryFlag
	{
		None = 0,

		UseDefaultLocation = 1 << 0,
		CreateIfDoesNotExist = 1 << 1,
	};
	enum class SHGetFileIconFlag
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
	enum class SHExexuteFlag
	{
		None = 0,

		Async = 1 << 0,
		HideUI = 1 << 1,
		InheritConsole = 1 << 2,
	};

	Kx_DeclareFlagSet(SHOperationFlags);
	Kx_DeclareFlagSet(SHGetKnownDirectoryFlag);
	Kx_DeclareFlagSet(SHGetFileIconFlag);
	Kx_DeclareFlagSet(SHExexuteFlag);
}

namespace kxf::Shell
{
	bool FileOperation(SHOperationType opType, const FSPath& source, const FSPath& destination, wxWindow* window = nullptr, FlagSet<SHOperationFlags> flags = {});
	bool FormatVolume(const wxWindow* window, const LegacyVolume& volume, bool quickFormat = false) noexcept;
	bool PinShortcut(const FSPath& filePath, SHPinShortcutCommand command);

	bool Execute(const wxWindow* window,
				 const FSPath& path,
				 const String& command = {},
				 const String& parameters = {},
				 const FSPath& workingDirectory = {},
				 FlagSet<SHWindowCommand> showWindow = SHWindowCommand::Show,
				 FlagSet<SHExexuteFlag> flags = {}
	);
	bool OpenURI(const wxWindow* window, const URI& uri, FlagSet<SHWindowCommand> showWindow = SHWindowCommand::Show, FlagSet<SHExexuteFlag> flags = {});
	HResult ExploreToItem(const FSPath& path);

	wxIcon GetFileIcon(const FSPath& path, FlagSet<SHGetFileIconFlag> flags);
	wxIcon GetFileIcon(const FileItem& item, FlagSet<SHGetFileIconFlag> flags);

	String QueryAssociation(const FSPath& filePath, SHQueryAssociation option, Any* extraData = nullptr);
	String QueryAssociation(const UniversallyUniqueID& classID, SHQueryAssociation option, Any* extraData = nullptr);

	String GetLocalizedName(const FSPath& path, int* resourceID = nullptr);
	HResult SetLocalizedName(const FSPath& path, const String& resourse, int resourceID);

	FSPath GetKnownDirectory(KnownDirectoryID id, FlagSet<SHGetKnownDirectoryFlag> flags = {});
	size_t EnumKnownDirectories(std::function<bool(KnownDirectoryID, String)> func);
}
