#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include "kxf/General/DateTime.h"
#include "kxf/System/UndefWindows.h"

namespace kxf
{
	class String;
}

namespace kxf
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

	enum class FileStreamAccess: uint32_t
	{
		None = 0,
		Read = 1 << 0,
		Write = 1 << 1,
		ReadAttributes = 1 << 2,
		WriteAttributes = 1 << 3,

		RW = Read|Write,
		AllAccess = RW|ReadAttributes|WriteAttributes
	};
	enum class FileStreamShare: uint32_t
	{
		None = 0,
		Read = 1 << 0,
		Write = 1 << 1,
		Delete = 1 << 2,

		Everything = Read|Write|Delete
	};
	enum class FileStreamDisposition: uint32_t
	{
		OpenExisting,
		OpenAlways,
		CreateNew,
		CreateAlways,
	};
	enum class FileStreamFlags: uint32_t
	{
		None = 0,

		Normal = 1 << 0,
		BackupSemantics = 1 << 1,
	};

	KxDeclareFlagSet(FileSystemFeature);
	KxDeclareFlagSet(FileAttribute);
	KxDeclareFlagSet(ReparsePointTag);

	KxDeclareFlagSet(FileStreamAccess);
	KxDeclareFlagSet(FileStreamShare);
	KxDeclareFlagSet(FileStreamFlags);
}

namespace kxf::FileSystem
{
	String GetNamespaceString(FSPathNamespace type);
	String GetForbiddenChars(const String& except = {});
}
