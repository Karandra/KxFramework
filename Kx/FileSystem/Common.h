#pragma once
#include "Kx/Common.hpp"
#include <KxFramework/KxEnumClassOperations.h>
#include <wx/wx.h>

namespace KxFileSystem
{
	using namespace KxEnumClassOperations;

	enum class PathNamespace
	{
		None = 0,
		NT,
		Win32File,
		Win32FileUNC,
		Win32Device,
		Win32Volume,
		Network,
		NetworkUNC,
	};
	enum class DriveType
	{
		Unknown = 0,
		NotMounted,
		Fixed,
		Remote,
		Memory,
		Optical,
		Removable,
	};
	enum class FileSystemFeature
	{
		None = 0,
		CasePreservedNames = 1 << 0,
		CaseSensitiveSearch = 1 << 1,
		FileCompression = 1 << 2,
		NamedStreams = 1 << 3,
		DirectAccess = 1 << 4,
		PersistentACLS = 1 << 5,
		ReadOnlyVolume = 1 << 6,
		CompressedVolume = 1 << 7,
		SequentialWrite = 1 << 8,
		ExtendedAttributes = 1 << 9,
		ReparsePoints = 1 << 10,
		HardLinks = 1 << 11,
		Encryption = 1 << 12,
		ObjectIDs = 1 << 13,
		OpenFileByID = 1 << 14,
		SparseFiles = 1 << 15,
		Transactions = 1 << 16,
		USNJournal = 1 << 17,
		Unicode = 1 << 18,
		VolumeQuotas = 1 << 19
	};
}

namespace KxFileSystem
{
	wxString GetNamespaceString(PathNamespace type);
	wxString GetForbiddenChars();
}

namespace KxEnumClassOperations
{
	KxImplementEnum(KxFileSystem::FileSystemFeature);
}
