#include "stdafx.h"
#include "NativeFileSystem.h"
#include "FileOperationEvent.h"
#include "Private/NativeFSUtility.h"
#include "kxf/General/ICoreApplication.h"
#include "kxf/General/AlignedStorage.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/IO/FileStream.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/CallAtScopeExit.h"

namespace
{
	using namespace kxf;

	bool OpenFileByID(const StorageVolume& volume,
					  const UniversallyUniqueID& fileID,
					  FileStream& file,
					  FlagSet<FileStreamAccess> access = FileStreamAccess::ReadAttributes,
					  FileStreamDisposition disposition = FileStreamDisposition::OpenExisting,
					  FlagSet<FileStreamShare> share = FileStreamShare::Everything,
					  FlagSet<FileStreamFlags> flags = FileStreamFlags::BackupSemantics)
	{
		if (fileID && volume)
		{
			FileStream volumeStream(volume.GetDevicePath(), FileStreamAccess::ReadAttributes, FileStreamDisposition::OpenExisting, FileStreamShare::Everything, FileStreamFlags::BackupSemantics);
			if (volumeStream)
			{
				// Some search on Google says that 'FILE_ID_DESCRIPTOR' isn't always 24. it *is* 24 for me on both x64 and x86
				// and it seems to work fine with the size of 24. Still I'm going to make it bigger, just in case.
				AlignedStorage<FILE_ID_DESCRIPTOR, 64, alignof(FILE_ID_DESCRIPTOR)> fileIDDescriptor;
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

				HANDLE handle = ::OpenFileById(volumeStream.GetHandle(),
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

	bool DoRemoveFile(const String& path) noexcept
	{
		return ::SetFileAttributesW(path.wc_str(), FILE_ATTRIBUTE_NORMAL) && ::DeleteFileW(path.wc_str());
	}
	bool DoRemoveDirectory(const String& path) noexcept
	{
		return ::SetFileAttributesW(path.wc_str(), FILE_ATTRIBUTE_NORMAL) && ::RemoveDirectoryW(path.wc_str());
	}

	template<class TFunc, class TNoneResult = std::invoke_result_t<TFunc, const FSPath&>>
	auto DoWithCurrentDirectory(const FSPath& currentDirectory, const FSPath& path, TFunc&& func, TNoneResult&& noneResult = {})
	{
		if (path.IsAbsolute())
		{
			return std::invoke(func, path);
		}
		else if (currentDirectory)
		{
			return std::invoke(func, currentDirectory / path);
		}
		return std::forward<TNoneResult>(noneResult);
	}

	template<class TFunc, class TNoneResult = std::invoke_result_t<TFunc, const FSPath&, const FSPath&>>
	auto DoWithCurrentDirectory2(const FSPath& currentDirectory, const FSPath& path1, const FSPath& path2, TFunc&& func, TNoneResult&& noneResult = {})
	{
		const bool isAbsolute1 = path1.IsAbsolute();
		const bool isAbsolute2 = path2.IsAbsolute();

		if (isAbsolute1 && isAbsolute2)
		{
			return std::invoke(func, path1, path2);
		}
		else if (currentDirectory)
		{
			return std::invoke(func, (isAbsolute1 ? path1 : currentDirectory / path1), (isAbsolute2 ? path2 : currentDirectory / path2));
		}
		return std::forward<TNoneResult>(noneResult);
	}
}

namespace kxf
{
	FSPath NativeFileSystem::GetExecutableDirectory()
	{
		return DynamicLibrary::GetExecutingModule().GetFilePath().GetParent();
	}
	FSPath NativeFileSystem::GetWorkingDirectory()
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
	bool NativeFileSystem::SetWorkingDirectory(const FSPath& directory)
	{
		if (directory)
		{
			auto DoChange = [](const FSPath& directory)
			{
				String directoryString = directory.GetFullPathWithNS(FSPathNamespace::Win32File);
				return ::SetCurrentDirectoryW(directoryString.wc_str());
			};

			if (ICoreApplication* app = ICoreApplication::GetInstance())
			{
				FileOperationEvent event(ICoreApplication::EvtWorkingDirectoryChanged);
				event.SetSource(GetWorkingDirectory());
				event.SetDestination(directory);
				event.Allow();

				if (app->GetEvtHandler().ProcessEvent(event) && !event.GetSkipped())
				{
					return event.IsAllowed() && DoChange(event.GetDestination());
				}
			}
			return DoChange(directory);
		}
		return false;
	}

	// IFileSystem
	bool NativeFileSystem::ItemExist(const FSPath& path) const
	{
		return DoWithCurrentDirectory(m_CurrentDirectory, path, [](const FSPath& path)
		{
			return FileSystem::Private::GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES;
		});
	}
	bool NativeFileSystem::FileExist(const FSPath& path) const
	{
		return DoWithCurrentDirectory(m_CurrentDirectory, path, [](const FSPath& path)
		{
			const FlagSet<DWORD> attributes = FileSystem::Private::GetFileAttributes(path);
			return !attributes.Equals(INVALID_FILE_ATTRIBUTES) && !attributes.Contains(FILE_ATTRIBUTE_DIRECTORY);
		});
	}
	bool NativeFileSystem::DirectoryExist(const FSPath& path) const
	{
		return DoWithCurrentDirectory(m_CurrentDirectory, path, [](const FSPath& path)
		{
			const FlagSet<uint32_t> attributes = FileSystem::Private::GetFileAttributes(path);
			return !attributes.Equals(INVALID_FILE_ATTRIBUTES) && attributes.Contains(FILE_ATTRIBUTE_DIRECTORY);
		});
	}

	FileItem NativeFileSystem::GetItem(const FSPath& path) const
	{
		return DoWithCurrentDirectory(m_CurrentDirectory, path, [](const FSPath& path) -> FileItem
		{
			FileStream file(path, FileStreamAccess::ReadAttributes, FileStreamDisposition::OpenExisting, FileStreamShare::Everything);
			if (file)
			{
				return FileSystem::Private::ConvertFileInfo(file.GetHandle());
			}
			return {};
		});
	}
	size_t NativeFileSystem::EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPathQuery& query, FlagSet<FSEnumItemsFlag> flags) const
	{
		return DoWithCurrentDirectory(m_CurrentDirectory, directory, [&](const FSPath& path) -> size_t
		{
			if (flags.Contains(FSEnumItemsFlag::LimitToFiles|FSEnumItemsFlag::LimitToDirectories))
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
			SearchDirectory(path, directories);

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
		});
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
		return DoWithCurrentDirectory(m_CurrentDirectory, path, [&](const FSPath& path)
		{
			if (FileSystem::Private::GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES)
			{
				FSPath finalPath;
				finalPath.ReserveLength(path.GetLength());
				finalPath.SetNamespace(path.GetNamespace());

				bool isCreated = false;
				path.ForEachComponent([&](String currentDirectoryName)
				{
					finalPath /= std::move(currentDirectoryName);

					String currentPath = finalPath.GetFullPathWithNS(FSPathNamespace::Win32File);
					isCreated = ::CreateDirectoryW(currentPath.wc_str(), nullptr);
					if (!isCreated && *Win32Error::GetLastError() != ERROR_ALREADY_EXISTS)
					{
						return false;
					}
					return true;
				});
				return isCreated;
			}
			return false;
		});
	}
	bool NativeFileSystem::ChangeAttributes(const FSPath& path, FlagSet<FileAttribute> attributes)
	{
		return DoWithCurrentDirectory(m_CurrentDirectory, path, [&](const FSPath& path) -> bool
		{
			if (attributes != FileAttribute::Invalid && path.IsAbsolute())
			{
				String pathString = path.GetFullPathWithNS(FSPathNamespace::Win32File);
				return ::SetFileAttributesW(pathString.wc_str(), FileSystem::Private::MapFileAttributes(attributes));
			}
			return false;
		});
	}
	bool NativeFileSystem::ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime)
	{
		return DoWithCurrentDirectory(m_CurrentDirectory, path, [&](const FSPath& path) -> bool
		{
			if ((creationTime.IsValid() || modificationTime.IsValid() || lastAccessTime.IsValid()))
			{
				const FileStreamFlags streamFlags = GetItem(path).IsDirectory() ? FileStreamFlags::BackupSemantics : FileStreamFlags::Normal;
				FileStream stream(path, FileStreamAccess::WriteAttributes, FileStreamDisposition::OpenExisting, FileStreamShare::Everything, streamFlags);
				if (stream)
				{
					return stream.ChangeTimestamp(creationTime, modificationTime, lastAccessTime);
				}
			}
			return false;
		});
	}

	bool NativeFileSystem::CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func, FlagSet<FSCopyItemFlag> flags)
	{
		return DoWithCurrentDirectory2(m_CurrentDirectory, source, destination, [&](const FSPath& source, const FSPath& destination)
		{
			BOOL cancel = FALSE;
			DWORD copyFlags = COPY_FILE_ALLOW_DECRYPTED_DESTINATION|COPY_FILE_COPY_SYMLINK;
			Utility::AddFlagRef(copyFlags, COPY_FILE_FAIL_IF_EXISTS, !(flags & FSCopyItemFlag::ReplaceIfExist));
			Utility::AddFlagRef(copyFlags, COPY_FILE_NO_BUFFERING, flags & FSCopyItemFlag::NoBuffering);

			const String sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
			const String destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);
			return ::CopyFileExW(sourcePath.wc_str(), destinationPath.wc_str(), FileSystem::Private::CopyCallback, func ? &func : nullptr, &cancel, copyFlags);
		});
	}
	bool NativeFileSystem::MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func, FlagSet<FSCopyItemFlag> flags)
	{
		return DoWithCurrentDirectory2(m_CurrentDirectory, source, destination, [&](const FSPath& source, const FSPath& destination)
		{
			DWORD moveFlags = MOVEFILE_COPY_ALLOWED;
			Utility::AddFlagRef(moveFlags, MOVEFILE_REPLACE_EXISTING, flags & FSCopyItemFlag::ReplaceIfExist);
			Utility::AddFlagRef(moveFlags, MOVEFILE_WRITE_THROUGH, flags & FSCopyItemFlag::NoBuffering);

			const String sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
			const String destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);
			return ::MoveFileWithProgressW(sourcePath.wc_str(), destinationPath.wc_str(), FileSystem::Private::CopyCallback, func ? &func : nullptr, moveFlags);
		});
	}
	bool NativeFileSystem::RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSCopyItemFlag> flags)
	{
		return DoWithCurrentDirectory2(m_CurrentDirectory, source, destination, [&](const FSPath& source, const FSPath& destination)
		{
			const String sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
			const String destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);

			DWORD moveFlags = flags & FSCopyItemFlag::ReplaceIfExist ? MOVEFILE_REPLACE_EXISTING : 0;
			return ::MoveFileExW(sourcePath.wc_str(), destinationPath.wc_str(), moveFlags);
		});
	}
	bool NativeFileSystem::RemoveItem(const FSPath& path)
	{
		return DoWithCurrentDirectory(m_CurrentDirectory, path, [&](const FSPath& path) -> bool
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
		});
	}
	bool NativeFileSystem::RemoveDirectory(const FSPath& path, FlagSet<FSEnumItemsFlag> flags)
	{
		return DoWithCurrentDirectory(m_CurrentDirectory, path, [&](const FSPath& path) -> bool
		{
			const String sourcePath = path.GetFullPathWithNS(FSPathNamespace::Win32File);

			const FlagSet<uint32_t> attributes = ::GetFileAttributesW(sourcePath.wc_str());
			if (!attributes.Equals(INVALID_FILE_ATTRIBUTES) && attributes.Contains(FILE_ATTRIBUTE_DIRECTORY))
			{
				if (flags.Contains(FSEnumItemsFlag::Recursive))
				{
					bool shouldExit = false;
					std::vector<String> directories;

					EnumItems(path, [&](FileItem item)
					{
						String filePath = item.GetFullPath().GetFullPathWithNS(FSPathNamespace::Win32File);
						if (item.IsDirectory())
						{
							if (DoRemoveDirectory(filePath))
							{
								return true;
							}
							else
							{
								if (*Win32Error::GetLastError() == ERROR_DIR_NOT_EMPTY)
								{
									// Defer deletion for later
									directories.emplace_back(std::move(filePath));
									return true;
								}
								shouldExit = true;
							}
						}
						else
						{
							shouldExit = !DoRemoveFile(filePath);
						}
						return !shouldExit;
					}, {}, FSEnumItemsFlag::Recursive);
					directories.emplace_back(path);

					size_t failCount = 0;
					while (!shouldExit)
					{
						// Not be the best solution but anyway
						if (failCount >= 10000)
						{
							return false;
						}

						for (const String& directory: directories)
						{
							if (!DoRemoveDirectory(directory))
							{
								failCount++;
								if (*Win32Error::GetLastError() != ERROR_DIR_NOT_EMPTY)
								{
									shouldExit = true;
									break;
								}
							}
						}
					}
					return !shouldExit;
				}
				else
				{
					return DoRemoveDirectory(sourcePath);
				}
			}
			return false;
		});
	}

	std::unique_ptr<wxInputStream> NativeFileSystem::OpenToRead(const FSPath& path) const
	{
		return DoWithCurrentDirectory(m_CurrentDirectory, path, [&](const FSPath& path) -> std::unique_ptr<wxInputStream>
		{
			auto stream = std::make_unique<FileStream>(path, FileStreamAccess::Read, FileStreamDisposition::OpenExisting, FileStreamShare::Read);
			if (stream->IsOk())
			{
				return stream;
			}
			return nullptr;
		});
	}
	std::unique_ptr<wxOutputStream> NativeFileSystem::OpenToWrite(const FSPath& path)
	{
		return DoWithCurrentDirectory(m_CurrentDirectory, path, [&](const FSPath& path) -> std::unique_ptr<wxOutputStream>
		{
			auto stream = std::make_unique<FileStream>(path, FileStreamAccess::Write, FileStreamDisposition::CreateAlways, FileStreamShare::Read|FileStreamShare::Write);
			if (stream->IsOk())
			{
				return stream;
			}
			return nullptr;
		});
	}

	// IFileIDSystem
	bool NativeFileSystem::ItemExist(const UniversallyUniqueID& id) const
	{
		FileStream file;
		return OpenFileByID(m_CurrentVolume, id, file);
	}
	bool NativeFileSystem::FileExist(const UniversallyUniqueID& id) const
	{
		FileStream file;
		if (OpenFileByID(m_CurrentVolume, id, file))
		{
			return !file.GetAttributes().Contains(FileAttribute::Directory);
		}
		return false;
	}
	bool NativeFileSystem::DirectoryExist(const UniversallyUniqueID& id) const
	{
		FileStream file;
		if (OpenFileByID(m_CurrentVolume, id, file))
		{
			return !file.GetAttributes().Contains(FileAttribute::Directory);
		}
		return false;
	}
	
	FileItem NativeFileSystem::GetItem(const UniversallyUniqueID& id) const
	{
		FileStream file;
		if (OpenFileByID(m_CurrentVolume, id, file))
		{
			return FileSystem::Private::ConvertFileInfo(file.GetHandle(), id);
		}
		return {};
	}

	std::unique_ptr<wxInputStream> NativeFileSystem::OpenToRead(const UniversallyUniqueID& id) const
	{
		if (id)
		{
			auto steram = std::make_unique<FileStream>();
			if (OpenFileByID(m_CurrentVolume, id, *steram, FileStreamAccess::Read, FileStreamDisposition::OpenExisting, FileStreamShare::Read, FileStreamFlags::None))
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
			if (OpenFileByID(m_CurrentVolume, id, *steram, FileStreamAccess::Write, FileStreamDisposition::CreateAlways, FileStreamShare::Read|FileStreamShare::Write, FileStreamFlags::None))
			{
				return steram;
			}
		}
		return nullptr;
	}

	// NativeFileSystem
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
}
