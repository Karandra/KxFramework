#include "stdafx.h"
#include "NativeFileSystem.h"
#include "Private/NativeFileSystem.h"
#include "FileStream.h"
#include "Kx/Utility/Common.h"
#include "Kx/Utility/CallAtScopeExit.h"

namespace KxFramework
{
	FileItem NativeFileSystem::GetItem(const FSPath& path) const
	{
		WIN32_FIND_DATAW findInfo = {};
		HANDLE handle = FileSystem::Private::CallFindFirstFile(path.GetFullPathWithNS(), findInfo);
		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			Utility::CallAtScopeExit atExit([&]()
			{
				::FindClose(handle);
			});

			if (FileSystem::Private::IsValidFindItem(findInfo))
			{
				return FileSystem::Private::ConvertFileInfo(findInfo, path);
			}
		}
		return {};
	}
	size_t NativeFileSystem::EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPathQuery& query, FSEnumItemsFlag flags) const
	{
		if (flags & FSEnumItemsFlag::LimitToFiles && flags & FSEnumItemsFlag::LimitToDirectories)
		{
			return 0;
		}

		size_t counter = 0;
		auto SearchDirectory = [&](const FSPath& directory, std::vector<FSPath>& childDirectories)
		{
			WIN32_FIND_DATAW findInfo = {};

			const String fullQuery = (directory / (!query ? wxS("*") : query)).GetFullPathWithNS(FSPathNamespace::Win32File);
			HANDLE handle = FileSystem::Private::CallFindFirstFile(fullQuery, findInfo, flags & FSEnumItemsFlag::CaseSensitive);
			if (handle && handle != INVALID_HANDLE_VALUE)
			{
				Utility::CallAtScopeExit atExit([&]()
				{
					::FindClose(handle);
				});

				do
				{
					// Skip invalid items and current and parent directory links
					if (FileSystem::Private::IsValidFindItem(findInfo))
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
						counter++;
						if (!std::invoke(func, FileSystem::Private::ConvertFileInfo(findInfo, directory)))
						{
							break;
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
			path.ForEachComponent([&](String currentDirectoryName)
			{
				finalPath /= std::move(currentDirectoryName);

				String currentPath = finalPath.GetFullPathWithNS(FSPathNamespace::Win32File);
				isCreated = ::CreateDirectoryW(currentPath.wc_str(), nullptr);
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
			String pathString = path.GetFullPathWithNS(FSPathNamespace::Win32File);
			return ::SetFileAttributesW(pathString.wc_str(), FileSystem::Private::MapFileAttributes(attributes));
		}
		return false;
	}
	bool NativeFileSystem::ChangeTimestamp(const FSPath& path, const wxDateTime& creationTime, const wxDateTime& modificationTime, const wxDateTime& lastAccessTime)
	{
		if (creationTime.IsValid() || modificationTime.IsValid() || lastAccessTime.IsValid())
		{
			const FileStreamFlags streamFlags = GetItem(path).IsDirectory() ? FileStreamFlags::BackupSemantics : FileStreamFlags::Normal;
			FileStream stream(path, FileStreamAccess::WriteAttributes, FileStreamDisposition::OpenExisting, FileStreamShare::Everything, streamFlags);
			if (stream)
			{
				return stream.ChangeTimestamp(creationTime, modificationTime, lastAccessTime);
			}
		}
		return false;
	}

	bool NativeFileSystem::CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func, FSCopyItemFlag flags)
	{
		BOOL cancel = FALSE;
		DWORD copyFlags = COPY_FILE_ALLOW_DECRYPTED_DESTINATION|COPY_FILE_COPY_SYMLINK;
		Utility::AddFlagRef(copyFlags, COPY_FILE_FAIL_IF_EXISTS, !(flags & FSCopyItemFlag::ReplaceIfExist));
		Utility::AddFlagRef(copyFlags, COPY_FILE_NO_BUFFERING, flags & FSCopyItemFlag::NoBuffering);

		const String sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
		const String destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);
		return ::CopyFileExW(sourcePath.wc_str(), destinationPath.wc_str(), FileSystem::Private::CopyCallback, func ? &func : nullptr, &cancel, copyFlags);
	}
	bool NativeFileSystem::MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func, FSCopyItemFlag flags)
	{
		DWORD moveFlags = MOVEFILE_COPY_ALLOWED;
		Utility::AddFlagRef(moveFlags, MOVEFILE_REPLACE_EXISTING, flags & FSCopyItemFlag::ReplaceIfExist);
		Utility::AddFlagRef(moveFlags, MOVEFILE_WRITE_THROUGH, flags & FSCopyItemFlag::NoBuffering);

		const String sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
		const String destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);
		return ::MoveFileWithProgressW(sourcePath.wc_str(), destinationPath.wc_str(), FileSystem::Private::CopyCallback, func ? &func : nullptr, moveFlags);
	}
	bool NativeFileSystem::RenameItem(const FSPath& source, const FSPath& destination, FSCopyItemFlag flags)
	{
		const String sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
		const String destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);

		DWORD moveFlags = flags & FSCopyItemFlag::ReplaceIfExist ? MOVEFILE_REPLACE_EXISTING : 0;
		return ::MoveFileExW(sourcePath.wc_str(), destinationPath.wc_str(), moveFlags);
	}
	bool NativeFileSystem::RemoveItem(const FSPath& path)
	{
		const String sourcePath = path.GetFullPathWithNS(FSPathNamespace::Win32File);

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
		return FileStream(path, FileStreamAccess::Read, FileStreamDisposition::OpenExisting, FileStreamShare::None).IsOk();
	}
	size_t NativeFileSystem::EnumStreams(const FSPath& path, TEnumStreamsFunc func) const
	{
		size_t counter = 0;
		String pathString = path.GetFullPathWithNS(FSPathNamespace::Win32File);

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
				counter++;
				if (!std::invoke(func, streamInfo.cStreamName, BinarySize::FromBytes(streamInfo.StreamSize.QuadPart)))
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
		return FileSystem::Private::CopyOrMoveDirectoryTree(const_cast<NativeFileSystem&>(*this), source, destination, std::move(func), flags, false);
	}
	bool NativeFileSystem::MoveDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func, FSCopyItemFlag flags)
	{
		return FileSystem::Private::CopyOrMoveDirectoryTree(*this, source, destination, std::move(func), flags, true);
	}

	FSPath NativeFileSystem::GetWorkingDirectory() const
	{
		DWORD length = ::GetCurrentDirectoryW(0, nullptr);
		if (length != 0)
		{
			String result;
			::GetCurrentDirectoryW(length, wxStringBuffer(result, length));

			return FSPath(result).EnsureNamespaceSet(FSPathNamespace::Win32File);
		}
		return {};
	}
	bool NativeFileSystem::SetWorkingDirectory(const FSPath& directory) const
	{
		String directoryString = directory.GetFullPathWithNS(FSPathNamespace::Win32File);
		return ::SetCurrentDirectoryW(directoryString.wc_str());
	}
}
