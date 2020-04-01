#pragma once
#include "Kx/Common.hpp"
#include <wx/wx.h>
#include "Kx/System/UndefWindows.h"

namespace KxFramework
{
	enum class DriveType: uint32_t
	{
		Unknown = 0,
		NotMounted,
		Fixed,
		Remote,
		Memory,
		Optical,
		Removable,
	};
	enum class FSPathNamespace: uint32_t
	{
		None = 0,
		NT,
		Win32File,
		Win32FileUNC,
		Win32Device,
		Network,
		NetworkUNC,
	};
	enum class FileSystemFeature: uint32_t
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
		VolumeQuotas = 1 << 19,
		LongFileNames = 1 << 20
	};
	enum class FileAttribute: uint32_t
	{
		None = 0,
		Invalid = std::numeric_limits<uint32_t>::max(),

		Hidden = 1 << 0,
		Archive = 1 << 1,
		Directory = 1 << 2,
		ReadOnly = 1 << 3,
		System = 1 << 4,
		Temporary = 1 << 5,
		Compressed = 1 << 6,
		Encrypted = 1 << 7,
		ReparsePoint = 1 << 8,
		SparseFile = 1 << 9,
		Offline = 1 << 10,
		ContentIndexed = 1 << 11,
		RecallOnOpen = 1 << 12,
		RecallOnDataAccess = 1 << 13,

		Normal = std::numeric_limits<uint32_t>::max() >> 1,
	};
	enum class ReparsePointTag: uint32_t
	{
		None = 0,

		MountPoint = 1 << 0,
		SymLink = 1 << 1
	};
}

namespace KxFramework::FileSystem
{
	wxString GetNamespaceString(FSPathNamespace type);

	wxString GetForbiddenChars();
	wxString GetForbiddenCharsExceptSeparators();
}

namespace KxEnumClassOperations
{
	KxImplementEnum(KxFramework::FileSystemFeature);
	KxImplementEnum(KxFramework::FileAttribute);
	KxImplementEnum(KxFramework::ReparsePointTag);
}
