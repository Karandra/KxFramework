#pragma once
#include "../Common.h"
#include "../FileItem.h"
#include "../FSPath.h"
#include "../NativeFileSystem.h"
#include "kxf/Utility/Common.h"

#include <Windows.h>
#include "kxf/System/UndefWindows.h"

namespace kxf::FileSystem::Private
{
	uint32_t GetFileAttributes(const FSPath& path);

	constexpr inline FlagSet<FileAttribute> MapFileAttributes(uint32_t nativeAttributes) noexcept
	{
		if (nativeAttributes == INVALID_FILE_ATTRIBUTES)
		{
			return FileAttribute::Invalid;
		}
		else if (nativeAttributes == FILE_ATTRIBUTE_NORMAL)
		{
			return FileAttribute::Normal;
		}
		else
		{
			FlagSet<FileAttribute> attributes;
			attributes.Add(FileAttribute::Hidden, nativeAttributes & FILE_ATTRIBUTE_HIDDEN);
			attributes.Add(FileAttribute::Archive, nativeAttributes & FILE_ATTRIBUTE_ARCHIVE);
			attributes.Add(FileAttribute::Directory, nativeAttributes & FILE_ATTRIBUTE_DIRECTORY);
			attributes.Add(FileAttribute::ReadOnly, nativeAttributes & FILE_ATTRIBUTE_READONLY);
			attributes.Add(FileAttribute::System, nativeAttributes & FILE_ATTRIBUTE_SYSTEM);
			attributes.Add(FileAttribute::Temporary, nativeAttributes & FILE_ATTRIBUTE_TEMPORARY);
			attributes.Add(FileAttribute::Compressed, nativeAttributes & FILE_ATTRIBUTE_COMPRESSED);
			attributes.Add(FileAttribute::Encrypted, nativeAttributes & FILE_ATTRIBUTE_ENCRYPTED);
			attributes.Add(FileAttribute::ReparsePoint, nativeAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
			attributes.Add(FileAttribute::SparseFile, nativeAttributes & FILE_ATTRIBUTE_SPARSE_FILE);
			attributes.Add(FileAttribute::Offline, nativeAttributes & FILE_ATTRIBUTE_OFFLINE);
			attributes.Add(FileAttribute::ContentIndexed, !(nativeAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED));
			attributes.Add(FileAttribute::RecallOnOpen, nativeAttributes & FILE_ATTRIBUTE_RECALL_ON_OPEN);
			attributes.Add(FileAttribute::RecallOnDataAccess, nativeAttributes & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS);

			return attributes;
		}
	}
	constexpr inline uint32_t MapFileAttributes(FlagSet<FileAttribute> attributes) noexcept
	{
		if (attributes == FileAttribute::Invalid)
		{
			return INVALID_FILE_ATTRIBUTES;
		}
		else if (attributes == FileAttribute::Normal)
		{
			return FILE_ATTRIBUTE_NORMAL;
		}
		else
		{
			int32_t nativeAttributes = 0;
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_HIDDEN, attributes & FileAttribute::Hidden);
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_ARCHIVE, attributes & FileAttribute::Archive);
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_DIRECTORY, attributes & FileAttribute::Directory);
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_READONLY, attributes & FileAttribute::ReadOnly);
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_SYSTEM, attributes & FileAttribute::System);
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_TEMPORARY, attributes & FileAttribute::Temporary);
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_COMPRESSED, attributes & FileAttribute::Compressed);
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_ENCRYPTED, attributes & FileAttribute::Encrypted);
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_REPARSE_POINT, attributes & FileAttribute::ReparsePoint);
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_SPARSE_FILE, attributes & FileAttribute::SparseFile);
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_OFFLINE, attributes & FileAttribute::Offline);
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, !(attributes & FileAttribute::ContentIndexed));
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_RECALL_ON_OPEN, attributes & FileAttribute::RecallOnOpen);
			Utility::AddFlagRef(nativeAttributes, FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS, attributes & FileAttribute::RecallOnDataAccess);

			return nativeAttributes;
		}
	}
	constexpr inline FlagSet<ReparsePointTag> MapReparsePointTags(uint32_t nativeTags) noexcept
	{
		// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-fscc/c8e77b37-3909-4fe6-a4ea-2b9d423b1ee4

		FlagSet<ReparsePointTag> tags;
		tags.Add(ReparsePointTag::MountPoint, nativeTags & IO_REPARSE_TAG_MOUNT_POINT);
		tags.Add(ReparsePointTag::SymLink, nativeTags & IO_REPARSE_TAG_SYMLINK);

		return tags;
	}

	inline DateTime ConvertDateTime(const SYSTEMTIME& systemTime) noexcept
	{
		return DateTime().SetSystemTime(systemTime, TimeZone::UTC);
	}
	inline DateTime ConvertDateTime(const FILETIME& fileTime) noexcept
	{
		return DateTime().SetFileTime(fileTime, TimeZone::UTC);
	}
	inline DateTime ConvertDateTime(const LARGE_INTEGER& fileTimeLI) noexcept
	{
		FILETIME fileTime = {};
		fileTime.dwLowDateTime = fileTimeLI.LowPart;
		fileTime.dwHighDateTime = fileTimeLI.HighPart;
		return ConvertDateTime(fileTime);
	}

	inline std::optional<SYSTEMTIME> ConvertDateTimeToSystemTime(DateTime dateTime) noexcept
	{
		if (dateTime)
		{
			return dateTime.GetSystemTime(TimeZone::UTC);
		}
		return {};
	}
	inline std::optional<FILETIME> ConvertDateTimeToFileTime(DateTime dateTime) noexcept
	{
		if (dateTime)
		{
			return dateTime.GetFileTime(TimeZone::UTC);
		}
		return {};
	}
	
	bool IsValidFindItem(const WIN32_FIND_DATAW& findInfo) noexcept;
	HANDLE CallFindFirstFile(const String& query, WIN32_FIND_DATAW& findInfo, bool isCaseSensitive = false);
	FileItem ConvertFileInfo(const WIN32_FIND_DATAW& findInfo, const FSPath& location, UniversallyUniqueID id = {}, bool forceFetchID = false);
	FileItem ConvertFileInfo(HANDLE fileHandle, UniversallyUniqueID id = {});

	DWORD CALLBACK CopyCallback(LARGE_INTEGER TotalFileSize,
								LARGE_INTEGER TotalBytesTransferred,
								LARGE_INTEGER StreamSize,
								LARGE_INTEGER StreamBytesTransferred,
								DWORD dwStreamNumber,
								DWORD dwCallbackReason,
								HANDLE hSourceFile,
								HANDLE hDestinationFile,
								LPVOID lpData);

	bool CopyOrMoveDirectoryTree(NativeFileSystem& fileSystem,
								 const FSPath& source,
								 const FSPath& destination,
								 NativeFileSystem::TCopyDirectoryTreeFunc func,
								 FlagSet<FSCopyItemFlag> flags,
								 bool move);
}
