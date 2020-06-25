#include "stdafx.h"
#include "NativeFileSystem.h"
#include "kxf/General/LocalPImpl.h"
#include "Private/NativeFSUtility.h"
#include "FileStream.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/CallAtScopeExit.h"

namespace
{
	using namespace kxf;

	bool OpenFileByID(const UniversallyUniqueID& fileID,
					  const UniversallyUniqueID& volumeID,
					  FileStream& file,
					  FlagSet<FileStreamAccess> access = FileStreamAccess::ReadAttributes,
					  FileStreamDisposition disposition = FileStreamDisposition::OpenExisting,
					  FlagSet<FileStreamShare> share = FileStreamShare::Everything,
					  FlagSet<FileStreamFlags> flags = FileStreamFlags::BackupSemantics)
	{
		if (fileID && volumeID)
		{
			FileStream volume(StorageVolume(volumeID).GetDevicePath(), FileStreamAccess::ReadAttributes, FileStreamDisposition::OpenExisting, FileStreamShare::Everything, FileStreamFlags::BackupSemantics);
			if (volume)
			{
				// Some search on Google says that 'FILE_ID_DESCRIPTOR' isn't always 24. it *is* 24 for me on both x64 and x86
				// and it seems to work fine with the size of 24. Still I'm going to make it bigger, just in case.
				LocalPImpl<FILE_ID_DESCRIPTOR, 64, alignof(FILE_ID_DESCRIPTOR)> fileIDDescriptor;
				fileIDDescriptor->dwSize = fileIDDescriptor.size();

				if (System::IsWindows8OrGreater())
				{
					fileIDDescriptor->Type = FILE_ID_TYPE::ExtendedFileIdType;

					auto uuid = fileID.ToInt128();
					std::memcpy(fileIDDescriptor->ExtendedFileId.Identifier, uuid.data(), uuid.size());
				}
				else
				{
					if (LocallyUniqueID luid = fileID.ToLocallyUniqueID())
					{
						fileIDDescriptor->Type = FILE_ID_TYPE::FileIdType;
						fileIDDescriptor->FileId.QuadPart = luid.ToInt();
					}
					else
					{
						return false;
					}
				}

				HANDLE handle = ::OpenFileById(volume.GetHandle(),
											   &fileIDDescriptor,
											   FileSystem::Private::MapFileAccessMode(access),
											   FileSystem::Private::MapFileShareMode(share),
											   nullptr,
											   FileSystem::Private::MapFileFlags(flags));
				if (handle && handle != INVALID_HANDLE_VALUE)
				{
					return file.AttachHandle(handle, access, disposition, share, flags);
				}
			}
		}
		return false;
	}
}

namespace kxf
{
	// IFileSystem
	bool NativeFileSystem::ItemExist(const FSPath& path) const
	{
		if (path.IsAbsolute())
		{
			return FileSystem::Private::GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES;
		}
		return false;
	}
	bool NativeFileSystem::FileExist(const FSPath& path) const
	{
		if (path.IsAbsolute())
		{
			const FlagSet<DWORD> attributes = FileSystem::Private::GetFileAttributes(path);
			return !attributes.Equals(INVALID_FILE_ATTRIBUTES) && !attributes.Contains(FILE_ATTRIBUTE_DIRECTORY);
		}
		return false;
	}
	bool NativeFileSystem::DirectoryExist(const FSPath& path) const
	{
		if (path.IsAbsolute())
		{
			const FlagSet<uint32_t> attributes = FileSystem::Private::GetFileAttributes(path);
			return !attributes.Equals(INVALID_FILE_ATTRIBUTES) && attributes.Contains(FILE_ATTRIBUTE_DIRECTORY);
		}
		return false;
	}

	FileItem NativeFileSystem::GetItem(const FSPath& path) const
	{
		if (path.IsAbsolute())
		{
			FileStream file(path, FileStreamAccess::ReadAttributes, FileStreamDisposition::OpenExisting, FileStreamShare::Everything);
			if (file)
			{
				return FileSystem::Private::ConvertFileInfo(file.GetHandle());
			}
		}
		return {};
	}
	size_t NativeFileSystem::EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPathQuery& query, FlagSet<FSEnumItemsFlag> flags) const
	{
		if (flags.Contains(FSEnumItemsFlag::LimitToFiles|FSEnumItemsFlag::LimitToDirectories) || directory.IsRelative())
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

						// Filter files and/or directories
						if (flags & FSEnumItemsFlag::LimitToFiles && isDirectory)
						{
							continue;
						}
						if (flags & FSEnumItemsFlag::LimitToDirectories && !isDirectory)
						{
							continue;
						}

						// Fetch the file info
						FileItem fileItem = FileSystem::Private::ConvertFileInfo(findInfo, directory);

						// Add directory to the stack if we need to scan child directories
						if (isDirectory && flags & FSEnumItemsFlag::Recursive)
						{
							childDirectories.emplace_back(fileItem.GetFullPath());
						}

						// Invoke the callback
						counter++;
						if (!std::invoke(func, std::move(fileItem)))
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
	bool NativeFileSystem::IsDirectoryEmpty(const FSPath& directory) const
	{
		return EnumItems(directory, [](const FileItem&)
		{
			return false;
		}, {}) == 0;
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
		if (attributes != FileAttribute::Invalid && path.IsAbsolute())
		{
			String pathString = path.GetFullPathWithNS(FSPathNamespace::Win32File);
			return ::SetFileAttributesW(pathString.wc_str(), FileSystem::Private::MapFileAttributes(attributes));
		}
		return false;
	}
	bool NativeFileSystem::ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime)
	{
		if ((creationTime.IsValid() || modificationTime.IsValid() || lastAccessTime.IsValid()) && path.IsAbsolute())
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

	bool NativeFileSystem::CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func, FlagSet<FSCopyItemFlag> flags)
	{
		if (source.IsAbsolute() && destination.IsAbsolute())
		{
			BOOL cancel = FALSE;
			DWORD copyFlags = COPY_FILE_ALLOW_DECRYPTED_DESTINATION|COPY_FILE_COPY_SYMLINK;
			Utility::AddFlagRef(copyFlags, COPY_FILE_FAIL_IF_EXISTS, !(flags & FSCopyItemFlag::ReplaceIfExist));
			Utility::AddFlagRef(copyFlags, COPY_FILE_NO_BUFFERING, flags & FSCopyItemFlag::NoBuffering);

			const String sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
			const String destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);
			return ::CopyFileExW(sourcePath.wc_str(), destinationPath.wc_str(), FileSystem::Private::CopyCallback, func ? &func : nullptr, &cancel, copyFlags);
		}
		return false;
	}
	bool NativeFileSystem::MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func, FlagSet<FSCopyItemFlag> flags)
	{
		if (source.IsAbsolute() && destination.IsAbsolute())
		{
			DWORD moveFlags = MOVEFILE_COPY_ALLOWED;
			Utility::AddFlagRef(moveFlags, MOVEFILE_REPLACE_EXISTING, flags & FSCopyItemFlag::ReplaceIfExist);
			Utility::AddFlagRef(moveFlags, MOVEFILE_WRITE_THROUGH, flags & FSCopyItemFlag::NoBuffering);

			const String sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
			const String destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);
			return ::MoveFileWithProgressW(sourcePath.wc_str(), destinationPath.wc_str(), FileSystem::Private::CopyCallback, func ? &func : nullptr, moveFlags);
		}
		return false;
	}
	bool NativeFileSystem::RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSCopyItemFlag> flags)
	{
		if (source.IsAbsolute() && destination.IsAbsolute())
		{
			const String sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
			const String destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);

			DWORD moveFlags = flags & FSCopyItemFlag::ReplaceIfExist ? MOVEFILE_REPLACE_EXISTING : 0;
			return ::MoveFileExW(sourcePath.wc_str(), destinationPath.wc_str(), moveFlags);
		}
		return false;
	}
	bool NativeFileSystem::RemoveItem(const FSPath& path)
	{
		if (path.IsAbsolute())
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
		}
		return false;
	}

	std::unique_ptr<wxInputStream> NativeFileSystem::OpenToRead(const FSPath& path)
	{
		if (path.IsAbsolute())
		{
			auto stream = std::make_unique<FileStream>(path, FileStreamAccess::Read, FileStreamDisposition::OpenExisting, FileStreamShare::Read);
			if (stream->IsOk())
			{
				return stream;
			}
		}
		return nullptr;
	}
	std::unique_ptr<wxOutputStream> NativeFileSystem::OpenToWrite(const FSPath& path)
	{
		if (path.IsAbsolute())
		{
			auto stream = std::make_unique<FileStream>(path, FileStreamAccess::Write, FileStreamDisposition::CreateAlways, FileStreamShare::Read|FileStreamShare::Write);
			if (stream->IsOk())
			{
				return stream;
			}
		}
		return nullptr;
	}

	// IFileIDSystem
	bool NativeFileSystem::ItemExist(const UniversallyUniqueID& id) const
	{
		FileStream file;
		return OpenFileByID(id, m_LookupScope, file);
	}
	bool NativeFileSystem::FileExist(const UniversallyUniqueID& id) const
	{
		FileStream file;
		if (OpenFileByID(id, m_LookupScope, file))
		{
			return !file.GetAttributes().Contains(FileAttribute::Directory);
		}
		return false;
	}
	bool NativeFileSystem::DirectoryExist(const UniversallyUniqueID& id) const
	{
		FileStream file;
		if (OpenFileByID(id, m_LookupScope, file))
		{
			return !file.GetAttributes().Contains(FileAttribute::Directory);
		}
		return false;
	}
	
	FileItem NativeFileSystem::GetItem(const UniversallyUniqueID& id) const
	{
		FileStream file;
		if (OpenFileByID(id, m_LookupScope, file))
		{
			return FileSystem::Private::ConvertFileInfo(file.GetHandle(), id);
		}
		return {};
	}

	std::unique_ptr<wxInputStream> NativeFileSystem::OpenToRead(const UniversallyUniqueID& id)
	{
		if (id)
		{
			auto steram = std::make_unique<FileStream>();
			if (OpenFileByID(id, m_LookupScope, *steram, FileStreamAccess::Read, FileStreamDisposition::OpenExisting, FileStreamShare::Read, FileStreamFlags::None))
			{
				return steram;
			}
		}
		return nullptr;
	}
	std::unique_ptr<wxOutputStream> NativeFileSystem::OpenToWrite(const UniversallyUniqueID& id)
	{
		if (id)
		{
			auto steram = std::make_unique<FileStream>();
			if (OpenFileByID(id, m_LookupScope, *steram, FileStreamAccess::Write, FileStreamDisposition::CreateAlways, FileStreamShare::Read|FileStreamShare::Write, FileStreamFlags::None))
			{
				return steram;
			}
		}
		return nullptr;
	}

	bool NativeFileSystem::IsInUse(const FSPath& path) const
	{
		return path.IsAbsolute() && FileStream(path, FileStreamAccess::Read, FileStreamDisposition::OpenExisting, FileStreamShare::None).IsOk();
	}
	size_t NativeFileSystem::EnumStreams(const FSPath& path, TEnumStreamsFunc func) const
	{
		if (path.IsAbsolute())
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
		return 0;
	}

	bool NativeFileSystem::RemoveDirectoryTree(const FSPath& path)
	{
		if (path.IsAbsolute())
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
		return false;
	}
	bool NativeFileSystem::CopyDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func, FlagSet<FSCopyItemFlag> flags) const
	{
		if (source.IsAbsolute() && destination.IsAbsolute())
		{
			return FileSystem::Private::CopyOrMoveDirectoryTree(const_cast<NativeFileSystem&>(*this), source, destination, std::move(func), flags, false);
		}
		return false;
	}
	bool NativeFileSystem::MoveDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func, FlagSet<FSCopyItemFlag> flags)
	{
		if (source.IsAbsolute() && destination.IsAbsolute())
		{
			return FileSystem::Private::CopyOrMoveDirectoryTree(*this, source, destination, std::move(func), flags, true);
		}
		return false;
	}

	FSPath NativeFileSystem::GetExecutableDirectory() const
	{
		return DynamicLibrary::GetExecutingModule().GetFilePath().GetParent();
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
