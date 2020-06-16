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
		SYSTEMTIME localTime = {};
		if (::SystemTimeToTzSpecificLocalTime(nullptr, &systemTime, &localTime))
		{
			return DateTime().SetSystemTime(localTime);
		}
		return wxInvalidDateTime;
	}
	inline DateTime ConvertDateTime(const FILETIME& fileTime) noexcept
	{
		if (fileTime.dwHighDateTime != 0 && fileTime.dwLowDateTime != 0)
		{
			SYSTEMTIME systemTime = {};
			if (::FileTimeToSystemTime(&fileTime, &systemTime))
			{
				return ConvertDateTime(systemTime);
			}
		}
		return wxInvalidDateTime;
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
	
	inline bool IsValidFindItem(const WIN32_FIND_DATAW& findInfo) noexcept
	{
		std::wstring_view name = findInfo.cFileName;
		return !(findInfo.dwFileAttributes == INVALID_FILE_ATTRIBUTES || name.empty() || name == L".." || name == L".");
	}
	inline HANDLE CallFindFirstFile(const String& query, WIN32_FIND_DATAW& findInfo, bool isCaseSensitive = false)
	{
		const DWORD searchFlags = FIND_FIRST_EX_LARGE_FETCH|(isCaseSensitive ? FIND_FIRST_EX_CASE_SENSITIVE : 0);
		return ::FindFirstFileExW(query.wc_str(), FindExInfoBasic, &findInfo, FINDEX_SEARCH_OPS::FindExSearchNameMatch, nullptr, searchFlags);
	}
	inline FileItem ConvertFileInfo(const WIN32_FIND_DATAW& findInfo, const FSPath& location)
	{
		FileItem fileItem;
		const bool isDirectory = findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

		// Construct path
		FSPath path;
		if (location.IsAbsolute())
		{
			path = isDirectory ? location : location.GetParent();
			path /= findInfo.cFileName;
			path.EnsureNamespaceSet(location.GetNamespace());
		}
		else
		{
			// Invalid operation, we need full path to parent directory
			return {};
		}

		// Attributes and reparse point
		fileItem.SetAttributes(MapFileAttributes(findInfo.dwFileAttributes));
		if (findInfo.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
		{
			fileItem.SetReparsePointTags(MapReparsePointTags(findInfo.dwReserved0));
		}

		// File size
		if (!isDirectory)
		{
			ULARGE_INTEGER size = {};
			size.HighPart = findInfo.nFileSizeHigh;
			size.LowPart = findInfo.nFileSizeLow;

			fileItem.SetSize(BinarySize::FromBytes(size.QuadPart));
		}

		// Compressed file size
		if (findInfo.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
		{
			ULARGE_INTEGER compressedSize = {};

			String pathName = path.GetFullPathWithNS();
			compressedSize.LowPart = ::GetCompressedFileSizeW(pathName.wc_str(), &compressedSize.HighPart);
			fileItem.SetCompressedSize(BinarySize::FromBytes(compressedSize.QuadPart));
		}

		// Date and time
		fileItem.SetCreationTime(ConvertDateTime(findInfo.ftCreationTime));
		fileItem.SetModificationTime(ConvertDateTime(findInfo.ftLastWriteTime));
		fileItem.SetLastAccessTime(ConvertDateTime(findInfo.ftLastAccessTime));

		// Assign path
		fileItem.SetFullPath(std::move(path));

		return fileItem;
	}

	inline DWORD CALLBACK CopyCallback(LARGE_INTEGER TotalFileSize,
									   LARGE_INTEGER TotalBytesTransferred,
									   LARGE_INTEGER StreamSize,
									   LARGE_INTEGER StreamBytesTransferred,
									   DWORD dwStreamNumber,
									   DWORD dwCallbackReason,
									   HANDLE hSourceFile,
									   HANDLE hDestinationFile,
									   LPVOID lpData)
	{
		auto& func = *reinterpret_cast<IFileSystem::TCopyItemFunc*>(lpData);
		if (func == nullptr || std::invoke(func, BinarySize::FromBytes(TotalBytesTransferred.QuadPart), BinarySize::FromBytes(TotalFileSize.QuadPart)))
		{
			return PROGRESS_CONTINUE;
		}
		return PROGRESS_CANCEL;
	}

	inline bool CopyOrMoveDirectoryTree(NativeFileSystem& fileSystem,
										const FSPath& source,
										const FSPath& destination,
										NativeFileSystem::TCopyDirectoryTreeFunc func,
										FlagSet<FSCopyItemFlag> flags, bool move)
	{
		return fileSystem.EnumItems(source, [&](FileItem item)
		{
			FSPath target = destination / item.GetFullPath().GetAfter(source);
			if (item.IsDirectory())
			{
				if (!func || std::invoke(func, source, target, 0, 0))
				{
					fileSystem.CreateDirectory(target);
					if (move)
					{
						fileSystem.RemoveItem(source);
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				if (func)
				{
					auto ForwardCallback = [&](BinarySize copied, BinarySize total)
					{
						return std::invoke(func, source, std::move(target), copied, total);
					};
					return move ? fileSystem.MoveItem(source, target, std::move(ForwardCallback), flags) : fileSystem.CopyItem(source, target, std::move(ForwardCallback), flags);
				}
				else
				{
					return move ? fileSystem.MoveItem(source, target, {}, flags) : fileSystem.CopyItem(source, target, {}, flags);
				}
			}
			return true;
		}, {}, FSEnumItemsFlag::Recursive) != 0;
	}
}
