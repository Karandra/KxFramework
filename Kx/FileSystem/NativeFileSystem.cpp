#include "KxStdAfx.h"
#include "NativeFileSystem.h"
#include "Kx/Utility/Common.h"
#include "Kx/Utility/CallAtScopeExit.h"
#include <KxFramework/KxFileStream.h>

namespace KxFramework
{
	HANDLE CallFindFirstFile(const wxString& query, WIN32_FIND_DATAW& findInfo, bool isCaseSensitive = false)
	{
		const DWORD searchFlags = FIND_FIRST_EX_LARGE_FETCH|(isCaseSensitive ? FIND_FIRST_EX_CASE_SENSITIVE : 0);
		return ::FindFirstFileExW(query.wc_str(), FindExInfoBasic, &findInfo, FINDEX_SEARCH_OPS::FindExSearchNameMatch, nullptr, searchFlags);
	}

	FileAttribute MapFileAttributes(uint32_t nativeAttributes)
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
			FileAttribute attributes = FileAttribute::None;
			Utility::ModFlagRef(attributes, FileAttribute::Hidden, nativeAttributes & FILE_ATTRIBUTE_HIDDEN);
			Utility::ModFlagRef(attributes, FileAttribute::Archive, nativeAttributes & FILE_ATTRIBUTE_ARCHIVE);
			Utility::ModFlagRef(attributes, FileAttribute::Directory, nativeAttributes & FILE_ATTRIBUTE_DIRECTORY);
			Utility::ModFlagRef(attributes, FileAttribute::ReadOnly, nativeAttributes & FILE_ATTRIBUTE_READONLY);
			Utility::ModFlagRef(attributes, FileAttribute::System, nativeAttributes & FILE_ATTRIBUTE_SYSTEM);
			Utility::ModFlagRef(attributes, FileAttribute::Temporary, nativeAttributes & FILE_ATTRIBUTE_TEMPORARY);
			Utility::ModFlagRef(attributes, FileAttribute::Compressed, nativeAttributes & FILE_ATTRIBUTE_COMPRESSED);
			Utility::ModFlagRef(attributes, FileAttribute::Encrypted, nativeAttributes & FILE_ATTRIBUTE_ENCRYPTED);
			Utility::ModFlagRef(attributes, FileAttribute::ReparsePoint, nativeAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
			Utility::ModFlagRef(attributes, FileAttribute::SparseFile, nativeAttributes & FILE_ATTRIBUTE_SPARSE_FILE);
			Utility::ModFlagRef(attributes, FileAttribute::Offline, nativeAttributes & FILE_ATTRIBUTE_OFFLINE);
			Utility::ModFlagRef(attributes, FileAttribute::ContentIndexed, !(nativeAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED));
			Utility::ModFlagRef(attributes, FileAttribute::RecallOnOpen, nativeAttributes & FILE_ATTRIBUTE_RECALL_ON_OPEN);
			Utility::ModFlagRef(attributes, FileAttribute::RecallOnDataAccess, nativeAttributes & FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS);

			return attributes;
		}
	}
	uint32_t MapFileAttributes(FileAttribute attributes)
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
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_HIDDEN, attributes & FileAttribute::Hidden);
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_ARCHIVE, attributes & FileAttribute::Archive);
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_DIRECTORY, attributes & FileAttribute::Directory);
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_READONLY, attributes & FileAttribute::ReadOnly);
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_SYSTEM, attributes & FileAttribute::System);
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_TEMPORARY, attributes & FileAttribute::Temporary);
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_COMPRESSED, attributes & FileAttribute::Compressed);
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_ENCRYPTED, attributes & FileAttribute::Encrypted);
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_REPARSE_POINT, attributes & FileAttribute::ReparsePoint);
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_SPARSE_FILE, attributes & FileAttribute::SparseFile);
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_OFFLINE, attributes & FileAttribute::Offline);
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, !(attributes & FileAttribute::ContentIndexed));
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_RECALL_ON_OPEN, attributes & FileAttribute::RecallOnOpen);
			Utility::ModFlagRef(nativeAttributes, FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS, attributes & FileAttribute::RecallOnDataAccess);

			return nativeAttributes;
		}
	}
	
	ReparsePointTag MapReparsePointTags(uint32_t nativeTags)
	{
		// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-fscc/c8e77b37-3909-4fe6-a4ea-2b9d423b1ee4

		ReparsePointTag tags = ReparsePointTag::None;
		Utility::ModFlagRef(tags, ReparsePointTag::MountPoint, nativeTags & IO_REPARSE_TAG_MOUNT_POINT);
		Utility::ModFlagRef(tags, ReparsePointTag::SymLink, nativeTags & IO_REPARSE_TAG_SYMLINK);

		return tags;
	}
	wxDateTime ConvertDateTime(const FILETIME& fileTime)
	{
		if (fileTime.dwHighDateTime != 0 && fileTime.dwLowDateTime != 0)
		{
			SYSTEMTIME systemTime = {0};
			SYSTEMTIME localTime = {0};
			if (::FileTimeToSystemTime(&fileTime, &systemTime) && ::SystemTimeToTzSpecificLocalTime(nullptr, &systemTime, &localTime))
			{
				return wxDateTime().SetFromMSWSysTime(localTime);
			}
		}
		return wxInvalidDateTime;
	}
	FileItem ConvertFileInfo(const WIN32_FIND_DATAW& findInfo, const FSPath& location)
	{
		FileItem fileItem;
		
		// Construct path
		FSPath fsPath(findInfo.cFileName);
		fsPath.EnsureNamespaceSet(location.GetNamespace());

		// Attributes and reparse point
		fileItem.SetAttributes(MapFileAttributes(findInfo.dwFileAttributes));
		if (findInfo.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
		{
			fileItem.SetReparsePointTags(MapReparsePointTags(findInfo.dwReserved0));
		}

		// File size
		if (!(findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			ULARGE_INTEGER size = {0};
			size.HighPart = findInfo.nFileSizeHigh;
			size.LowPart = findInfo.nFileSizeLow;

			fileItem.SetSize(BinarySize::FromBytes(size.QuadPart));
		}

		// Compressed file size
		if (findInfo.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
		{
			ULARGE_INTEGER compressedSize = {};

			wxString path = fsPath.GetFullPathWithNS();
			compressedSize.LowPart = ::GetCompressedFileSizeW(path.wc_str(), &compressedSize.HighPart);
			fileItem.SetCompressedSize(BinarySize::FromBytes(compressedSize.QuadPart));
		}

		// Date and time
		fileItem.SetCreationTime(ConvertDateTime(findInfo.ftCreationTime));
		fileItem.SetModificationTime(ConvertDateTime(findInfo.ftLastWriteTime));
		fileItem.SetLastAccessTime(ConvertDateTime(findInfo.ftLastAccessTime));

		// Assign path
		fileItem.SetFullPath(std::move(fsPath));

		return fileItem;
	}
	bool IsValidFindItem(const WIN32_FIND_DATAW& findInfo)
	{
		std::wstring_view name = findInfo.cFileName;
		return !(findInfo.dwFileAttributes == INVALID_FILE_ATTRIBUTES || name.empty() || name == L".." || name == L".");
	}

	DWORD WINAPI CopyCallback(LARGE_INTEGER TotalFileSize,
							  LARGE_INTEGER TotalBytesTransferred,
							  LARGE_INTEGER StreamSize,
							  LARGE_INTEGER StreamBytesTransferred,
							  DWORD dwStreamNumber,
							  DWORD dwCallbackReason,
							  HANDLE hSourceFile,
							  HANDLE hDestinationFile,
							  LPVOID lpData)
	{
		IFileSystem::TCopyItemFunc& func = *reinterpret_cast<IFileSystem::TCopyItemFunc*>(lpData);
		if (func == nullptr || std::invoke(func, BinarySize::FromBytes(TotalBytesTransferred.QuadPart), BinarySize::FromBytes(TotalFileSize.QuadPart)))
		{
			return PROGRESS_CONTINUE;
		}
		return PROGRESS_CANCEL;
	}

	bool CopyOrMoveDirectoryTree(NativeFileSystem& fileSystem, const FSPath& source, const FSPath& destination, NativeFileSystem::TCopyDirectoryTreeFunc func, FSCopyItemFlag flags, bool move)
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

namespace KxFramework
{
	FileItem NativeFileSystem::GetItem(const FSPath& path) const
	{
		WIN32_FIND_DATAW findInfo = {};
		HANDLE handle = CallFindFirstFile(path.GetFullPathWithNS(), findInfo);
		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			Utility::CallAtScopeExit atExit([&]()
			{
				::FindClose(handle);
			});

			if (IsValidFindItem(findInfo))
			{
				return ConvertFileInfo(findInfo, path);
			}
		}
		return {};
	}
	size_t NativeFileSystem::EnumItems(const FSPath& directory, TEnumItemsFunc func, const wxString& query, FSEnumItemsFlag flags) const
	{
		if (flags & FSEnumItemsFlag::LimitToFiles && flags & FSEnumItemsFlag::LimitToDirectories)
		{
			return 0;
		}

		size_t counter = 0;
		auto SearchDirectory = [&](const FSPath& directory, std::vector<FSPath>& childDirectories)
		{
			WIN32_FIND_DATAW findInfo = {};

			const wxString fullQuery = (directory / (query.IsEmpty() ? wxS("*") : query)).GetFullPathWithNS(FSPathNamespace::Win32File);
			HANDLE handle = CallFindFirstFile(fullQuery, findInfo, flags & FSEnumItemsFlag::CaseSensitive);
			if (handle && handle != INVALID_HANDLE_VALUE)
			{
				Utility::CallAtScopeExit atExit([&]()
				{
					::FindClose(handle);
				});

				do
				{
					// Skip invalid items and current and parent directory links
					if (IsValidFindItem(findInfo))
					{
						const bool isDirectory = findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

						// Add directory to the stack if we need to scan child directories
						if (isDirectory && flags & FSEnumItemsFlag::Recursive)
						{
							childDirectories.emplace_back(findInfo.cFileName).EnsureNamespaceSet(directory.GetNamespace());
						}

						// Filter files and/or directories
						if (flags & FSEnumItemsFlag::LimitToFiles && isDirectory)
						{
							continue;
						}
						if (flags & FSEnumItemsFlag::LimitToDirectories && !isDirectory)
						{
							continue;
						}

						// Fetch the file info and invoke the callback
						if (std::invoke(func, ConvertFileInfo(findInfo, directory)))
						{
							counter++;
						}
						else
						{
							return;
						}
					}
				}
				while (::FindNextFileW(handle, &findInfo));
			}
		};
		
		std::vector<FSPath> directories;
		SearchDirectory(directory, directories);

		while (!directories.empty())
		{
			std::vector<FSPath> roundDirectories;
			for (const FSPath& path: directories)
			{
				SearchDirectory(path, roundDirectories);
			}
			directories = std::move(roundDirectories);
		}
		return counter;
	}
	
	bool NativeFileSystem::CreateDirectory(const FSPath& path)
	{
		if (!GetItem(path))
		{
			FSPath finalPath;
			finalPath.SetNamespace(path.GetNamespace());

			bool isCreated = false;
			path.ForEachComponent([&](const wxString& part)
			{
				finalPath /= part;

				wxString path = finalPath.GetFullPathWithNS(FSPathNamespace::Win32File);
				isCreated = ::CreateDirectoryW(path.wc_str(), nullptr);
				return true;
			});
			return isCreated;
		}
		return false;
	}
	bool NativeFileSystem::ChangeAttributes(const FSPath& path, FileAttribute attributes)
	{
		if (attributes != FileAttribute::Invalid)
		{
			wxString pathString = path.GetFullPathWithNS(FSPathNamespace::Win32File);
			return ::SetFileAttributesW(pathString.wc_str(), MapFileAttributes(attributes));
		}
		return false;
	}
	bool NativeFileSystem::ChangeTimestamp(const FSPath& path, const wxDateTime& creationTime, const wxDateTime& modificationTime, const wxDateTime& lastAccessTime)
	{
		if (creationTime.IsValid() || modificationTime.IsValid() || lastAccessTime.IsValid())
		{
			const KxFileStream::Flags streamFlags = GetItem(path).IsDirectory() ? KxFileStream::Flags::BackupSemantics : KxFileStream::Flags::Normal;
			KxFileStream stream(path, KxFileStream::Access::WriteAttributes, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Everything, streamFlags);
			if (stream)
			{
				return stream.SetFileTime(creationTime, modificationTime, lastAccessTime);
			}
		}
		return false;
	}

	bool NativeFileSystem::CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func, FSCopyItemFlag flags)
	{
		BOOL cancel = FALSE;
		DWORD copyFlags = COPY_FILE_ALLOW_DECRYPTED_DESTINATION|COPY_FILE_COPY_SYMLINK;
		Utility::ModFlagRef(copyFlags, COPY_FILE_FAIL_IF_EXISTS, !(flags & FSCopyItemFlag::ReplaceIfExist));
		Utility::ModFlagRef(copyFlags, COPY_FILE_NO_BUFFERING, flags & FSCopyItemFlag::NoBuffering);

		const wxString sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
		const wxString destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);
		return ::CopyFileExW(sourcePath.wc_str(), destinationPath.wc_str(), CopyCallback, func ? &func : nullptr, &cancel, copyFlags);
	}
	bool NativeFileSystem::MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func, FSCopyItemFlag flags)
	{
		DWORD moveFlags = MOVEFILE_COPY_ALLOWED;
		Utility::ModFlagRef(moveFlags, MOVEFILE_REPLACE_EXISTING, flags & FSCopyItemFlag::ReplaceIfExist);
		Utility::ModFlagRef(moveFlags, MOVEFILE_WRITE_THROUGH, flags & FSCopyItemFlag::NoBuffering);

		const wxString sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
		const wxString destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);
		return ::MoveFileWithProgressW(sourcePath.wc_str(), destinationPath.wc_str(), CopyCallback, func ? &func : nullptr, moveFlags);
	}
	bool NativeFileSystem::RenameItem(const FSPath& source, const FSPath& destination, FSCopyItemFlag flags)
	{
		const wxString sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
		const wxString destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);

		DWORD moveFlags = flags & FSCopyItemFlag::ReplaceIfExist ? MOVEFILE_REPLACE_EXISTING : 0;
		return ::MoveFileExW(sourcePath.wc_str(), destinationPath.wc_str(), moveFlags);
	}
	bool NativeFileSystem::RemoveItem(const FSPath& path)
	{
		const wxString sourcePath = path.GetFullPathWithNS(FSPathNamespace::Win32File);

		const uint32_t attributes = ::GetFileAttributesW(sourcePath.wc_str());
		if (attributes != INVALID_FILE_ATTRIBUTES)
		{
			if (::SetFileAttributesW(sourcePath.wc_str(), FILE_ATTRIBUTE_NORMAL))
			{
				if (attributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					return ::RemoveDirectoryW(sourcePath.wc_str());
				}
				else
				{
					return ::DeleteFileW(sourcePath.wc_str());
				}
			}
		}
		return false;
	}

	bool NativeFileSystem::IsInUse(const FSPath& path) const
	{
		return KxFileStream(path, KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Exclusive).IsOk();
	}
	size_t NativeFileSystem::EnumStreams(const FSPath& path, TEnumStreamsFunc func) const
	{
		size_t counter = 0;
		wxString pathString = path.GetFullPathWithNS(FSPathNamespace::Win32File);

		WIN32_FIND_STREAM_DATA streamInfo = {};
		HANDLE handle = ::FindFirstStreamW(pathString.wc_str(), STREAM_INFO_LEVELS::FindStreamInfoStandard, &streamInfo, 0);
		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			Utility::CallAtScopeExit atExit([&]()
			{
				::FindClose(handle);
			});

			do
			{
				// Fetch the file info and invoke the callback
				if (std::invoke(func, streamInfo.cStreamName, BinarySize::FromBytes(streamInfo.StreamSize.QuadPart)))
				{
					counter++;
				}
				else
				{
					break;
				}
			}
			while (::FindNextStreamW(handle, &streamInfo));
		}
		return counter;
	}

	bool NativeFileSystem::RemoveDirectoryTree(const FSPath& path)
	{
		std::vector<FSPath> directories;
		EnumItems(path, [&](FileItem item)
		{
			if (item.IsDirectory())
			{
				directories.emplace_back(item.GetFullPath());
			}
			else
			{
				return RemoveItem(item.GetFullPath());
			}
			return true;
		}, {}, FSEnumItemsFlag::Recursive);

		for (const FSPath& directory: directories)
		{
			return RemoveItem(directory);
		}
		return RemoveItem(path);
	}
	bool NativeFileSystem::CopyDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func, FSCopyItemFlag flags) const
	{
		return CopyOrMoveDirectoryTree(const_cast<NativeFileSystem&>(*this), source, destination, std::move(func), flags, false);
	}
	bool NativeFileSystem::MoveDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func, FSCopyItemFlag flags)
	{
		return CopyOrMoveDirectoryTree(*this, source, destination, std::move(func), flags, true);
	}

	FSPath NativeFileSystem::GetWorkingDirectory() const
	{
		DWORD length = ::GetCurrentDirectoryW(0, nullptr);
		if (length != 0)
		{
			wxString result;
			::GetCurrentDirectoryW(length, wxStringBuffer(result, length));

			return FSPath(result).EnsureNamespaceSet(FSPathNamespace::Win32File);
		}
		return {};
	}
	bool NativeFileSystem::SetWorkingDirectory(const FSPath& directory) const
	{
		wxString directoryString = directory.GetFullPathWithNS(FSPathNamespace::Win32File);
		return ::SetCurrentDirectoryW(directoryString.wc_str());
	}
}
