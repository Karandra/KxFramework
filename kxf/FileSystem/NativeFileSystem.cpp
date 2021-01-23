#include "stdafx.h"
#include "NativeFileSystem.h"
#include "FSActionEvent.h"
#include "Private/NativeFSUtility.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/General/AlignedStorage.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/IO/NativeFileStream.h"
#include "kxf/Utility/Common.h"
#include "kxf/Utility/ScopeGuard.h"
#include <wx/filename.h>

namespace
{
	using namespace kxf;

	bool OpenFileByID(const StorageVolume& volume,
					  const UniversallyUniqueID& fileID,
					  NativeFileStream& file,
					  FlagSet<IOStreamAccess> access = IOStreamAccess::ReadAttributes,
					  IOStreamDisposition disposition = IOStreamDisposition::OpenExisting,
					  FlagSet<IOStreamShare> share = IOStreamShare::Everything,
					  FlagSet<IOStreamFlag> flags = IOStreamFlag::AllowDirectories)
	{
		if (fileID && volume)
		{
			NativeFileStream volumeStream(volume.GetDevicePath(), IOStreamAccess::ReadAttributes, IOStreamDisposition::OpenExisting, IOStreamShare::Everything, IOStreamFlag::AllowDirectories);
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
					return file.AttachHandle(handle);
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
	auto DoWithResolvedPath1(const FSPath& currentDirectory, const FSPath& path, TFunc&& func, TNoneResult&& noneResult = {})
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
	auto DoWithResolvedPath2(const FSPath& currentDirectory, const FSPath& path1, const FSPath& path2, TFunc&& func, TNoneResult&& noneResult = {})
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
	FSPath NativeFileSystem::GetExecutingModuleRootDirectory()
	{
		return DynamicLibrary::GetExecutingModule().GetFilePath().GetParent();
	}
	FSPath NativeFileSystem::GetExecutingModuleWorkingDirectory()
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
	bool NativeFileSystem::SetExecutingModuleWorkingDirectory(const FSPath& directory)
	{
		if (directory)
		{
			auto DoChange = [](const FSPath& directory)
			{
				String directoryString = directory.GetFullPathWithNS(FSPathNamespace::Win32File);
				return ::SetCurrentDirectoryW(directoryString.wc_str());
			};

			if (auto app = ICoreApplication::GetInstance())
			{
				FSActionEvent event;
				event.SetSource(GetExecutingModuleWorkingDirectory());
				event.SetDestination(directory);
				event.Allow();

				if (app->ProcessEvent(event, ICoreApplication::EvtExecutingModuleWorkingDirectoryChanged) && !event.IsSkipped())
				{
					return event.IsAllowed() && DoChange(event.GetDestination());
				}
			}
			return DoChange(directory);
		}
		return false;
	}

	// IFileSystem
	bool NativeFileSystem::IsValidPathName(const FSPath& path) const
	{
		if (IsNull())
		{
			return false;
		}

		// All forbidden characters don't have case variants so we can use case-sensitive comparison
		return path && !path.ContainsAnyOfCharacters(wxFileName::GetForbiddenChars(), true);
	}
	String NativeFileSystem::GetForbiddenPathNameCharacters(const String& except) const
	{
		if (IsNull())
		{
			return {};
		}

		String forbiddenChars = wxFileName::GetForbiddenChars();
		for (XChar c : except)
		{
			forbiddenChars.Replace(c, NullString);
		}
		return forbiddenChars;
	}

	FSPath NativeFileSystem::ResolvePath(const FSPath& relativePath) const
	{
		if (IsNull())
		{
			return {};
		}

		return DoWithResolvedPath1(m_CurrentDirectory, relativePath, [](const FSPath& path)
		{
			return path;
		});
	}

	bool NativeFileSystem::ItemExist(const FSPath& path) const
	{
		if (IsNull())
		{
			return false;
		}

		return DoWithResolvedPath1(m_CurrentDirectory, path, [](const FSPath& path)
		{
			return FileSystem::Private::GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES;
		});
	}
	bool NativeFileSystem::FileExist(const FSPath& path) const
	{
		if (IsNull())
		{
			return false;
		}

		return DoWithResolvedPath1(m_CurrentDirectory, path, [](const FSPath& path)
		{
			const FlagSet<DWORD> attributes = FileSystem::Private::GetFileAttributes(path);
			return !attributes.Equals(INVALID_FILE_ATTRIBUTES) && !attributes.Contains(FILE_ATTRIBUTE_DIRECTORY);
		});
	}
	bool NativeFileSystem::DirectoryExist(const FSPath& path) const
	{
		if (IsNull())
		{
			return false;
		}

		return DoWithResolvedPath1(m_CurrentDirectory, path, [](const FSPath& path)
		{
			const FlagSet<uint32_t> attributes = FileSystem::Private::GetFileAttributes(path);
			return !attributes.Equals(INVALID_FILE_ATTRIBUTES) && attributes.Contains(FILE_ATTRIBUTE_DIRECTORY);
		});
	}

	FileItem NativeFileSystem::GetItem(const FSPath& path) const
	{
		if (IsNull())
		{
			return {};
		}

		return DoWithResolvedPath1(m_CurrentDirectory, path, [](const FSPath& path) -> FileItem
		{
			NativeFileStream file(path, IOStreamAccess::ReadAttributes, IOStreamDisposition::OpenExisting, IOStreamShare::Everything);
			if (file)
			{
				return FileSystem::Private::ConvertFileInfo(file.GetHandle());
			}
			return {};
		});
	}
	size_t NativeFileSystem::EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPath& query, FlagSet<FSActionFlag> flags) const
	{
		if (IsNull())
		{
			return 0;
		}

		return DoWithResolvedPath1(m_CurrentDirectory, directory, [&](const FSPath& path) -> size_t
		{
			if (flags.Contains(FSActionFlag::LimitToFiles|FSActionFlag::LimitToDirectories))
			{
				return 0;
			}

			FSPath resolvedRootDirectory;
			if (flags.Contains(FSActionFlag::RelativePath))
			{
				resolvedRootDirectory = path;
			}

			size_t counter = 0;
			auto SearchDirectory = [&](const FSPath& directory, std::vector<FSPath>& childDirectories)
			{
				WIN32_FIND_DATAW findInfo = {};

				const String fullQuery = (directory / (!query ? wxS("*") : query)).GetFullPathWithNS(FSPathNamespace::Win32File);
				HANDLE handle = FileSystem::Private::CallFindFirstFile(fullQuery, findInfo, flags & FSActionFlag::CaseSensitive);
				if (handle && handle != INVALID_HANDLE_VALUE)
				{
					Utility::ScopeGuard atExit([&]()
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
							if (isDirectory && flags & FSActionFlag::Recursive)
							{
								FSPath childDirectory = directory / findInfo.cFileName;
								childDirectory.EnsureNamespaceSet(directory.GetNamespace());
								childDirectories.emplace_back(std::move(childDirectory));
							}

							// Filter files and/or directories
							if (flags & FSActionFlag::LimitToFiles && isDirectory)
							{
								continue;
							}
							if (flags & FSActionFlag::LimitToDirectories && !isDirectory)
							{
								continue;
							}

							// Fetch the file info
							FileItem fileItem = FileSystem::Private::ConvertFileInfo(findInfo, directory, {}, flags);

							// Make final path relative if needed
							if (flags.Contains(FSActionFlag::RelativePath))
							{
								fileItem.SetFullPath(fileItem.GetFullPath().GetAfter(resolvedRootDirectory));
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
		if (IsNull())
		{
			return false;
		}

		return EnumItems(directory, [](const FileItem&)
		{
			return false;
		}, {}) == 0;
	}

	bool NativeFileSystem::CreateDirectory(const FSPath& path, FlagSet<FSActionFlag> flags)
	{
		if (IsNull())
		{
			return false;
		}

		return DoWithResolvedPath1(m_CurrentDirectory, path, [&](const FSPath& path) -> bool
		{
			if (flags.Contains(FSActionFlag::Recursive))
			{
				if (FileSystem::Private::GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES)
				{
					FSPath finalPath;
					finalPath.ReserveLength(path.GetLength());
					finalPath.SetNamespace(path.GetNamespace());

					bool result = false;
					path.ForEachComponent([&](String currentDirectoryName)
					{
						finalPath /= std::move(currentDirectoryName);

						const String currentPath = finalPath.GetFullPathWithNS(FSPathNamespace::Win32File);
						const bool isCreated = ::CreateDirectoryW(currentPath.wc_str(), nullptr);
						const bool alreadyExist = !isCreated && (*Win32Error::GetLastError() == ERROR_ALREADY_EXISTS || finalPath.GetComponentCount() == 1);

						if (!isCreated && !alreadyExist)
						{
							return false;
						}

						result = isCreated || alreadyExist;
						return true;
					});
					return result;
				}
			}
			else
			{
				const String pathName = path.GetFullPathWithNS(FSPathNamespace::Win32File);
				return ::CreateDirectoryW(pathName.wc_str(), nullptr);
			}
			return false;
		});
	}
	bool NativeFileSystem::ChangeAttributes(const FSPath& path, FlagSet<FileAttribute> attributes)
	{
		if (IsNull())
		{
			return false;
		}

		return DoWithResolvedPath1(m_CurrentDirectory, path, [&](const FSPath& path) -> bool
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
		if (IsNull())
		{
			return false;
		}

		return DoWithResolvedPath1(m_CurrentDirectory, path, [&](const FSPath& path) -> bool
		{
			if ((creationTime.IsValid() || modificationTime.IsValid() || lastAccessTime.IsValid()))
			{
				const IOStreamFlag streamFlags = GetItem(path).IsDirectory() ? IOStreamFlag::AllowDirectories : IOStreamFlag::Normal;
				NativeFileStream stream(path, IOStreamAccess::WriteAttributes, IOStreamDisposition::OpenExisting, IOStreamShare::Everything, streamFlags);
				if (stream)
				{
					return stream.ChangeTimestamp(creationTime, modificationTime, lastAccessTime);
				}
			}
			return false;
		});
	}

	bool NativeFileSystem::CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func, FlagSet<FSActionFlag> flags)
	{
		if (IsNull())
		{
			return false;
		}

		return DoWithResolvedPath2(m_CurrentDirectory, source, destination, [&](const FSPath& source, const FSPath& destination)
		{
			BOOL cancel = FALSE;
			DWORD copyFlags = COPY_FILE_ALLOW_DECRYPTED_DESTINATION|COPY_FILE_COPY_SYMLINK;
			Utility::AddFlagRef(copyFlags, COPY_FILE_FAIL_IF_EXISTS, !(flags & FSActionFlag::ReplaceIfExist));
			Utility::AddFlagRef(copyFlags, COPY_FILE_NO_BUFFERING, flags & FSActionFlag::NoBuffering);

			const String sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
			const String destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);
			return ::CopyFileExW(sourcePath.wc_str(), destinationPath.wc_str(), FileSystem::Private::CopyCallback, func ? &func : nullptr, &cancel, copyFlags);
		});
	}
	bool NativeFileSystem::MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func, FlagSet<FSActionFlag> flags)
	{
		if (IsNull())
		{
			return false;
		}

		return DoWithResolvedPath2(m_CurrentDirectory, source, destination, [&](const FSPath& source, const FSPath& destination)
		{
			DWORD moveFlags = MOVEFILE_COPY_ALLOWED;
			Utility::AddFlagRef(moveFlags, MOVEFILE_REPLACE_EXISTING, flags & FSActionFlag::ReplaceIfExist);
			Utility::AddFlagRef(moveFlags, MOVEFILE_WRITE_THROUGH, flags & FSActionFlag::NoBuffering);

			const String sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
			const String destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);
			return ::MoveFileWithProgressW(sourcePath.wc_str(), destinationPath.wc_str(), FileSystem::Private::CopyCallback, func ? &func : nullptr, moveFlags);
		});
	}
	bool NativeFileSystem::RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSActionFlag> flags)
	{
		if (IsNull())
		{
			return false;
		}

		return DoWithResolvedPath2(m_CurrentDirectory, source, destination, [&](const FSPath& source, const FSPath& destination)
		{
			const String sourcePath = source.GetFullPathWithNS(FSPathNamespace::Win32File);
			const String destinationPath = destination.GetFullPathWithNS(FSPathNamespace::Win32File);

			DWORD moveFlags = flags & FSActionFlag::ReplaceIfExist ? MOVEFILE_REPLACE_EXISTING : 0;
			return ::MoveFileExW(sourcePath.wc_str(), destinationPath.wc_str(), moveFlags);
		});
	}
	bool NativeFileSystem::RemoveItem(const FSPath& path)
	{
		if (IsNull())
		{
			return false;
		}

		return DoWithResolvedPath1(m_CurrentDirectory, path, [&](const FSPath& path) -> bool
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
	bool NativeFileSystem::RemoveDirectory(const FSPath& path, FlagSet<FSActionFlag> flags)
	{
		if (IsNull())
		{
			return false;
		}

		return DoWithResolvedPath1(m_CurrentDirectory, path, [&](const FSPath& path) -> bool
		{
			const String sourcePath = path.GetFullPathWithNS(FSPathNamespace::Win32File);

			const FlagSet<uint32_t> attributes = ::GetFileAttributesW(sourcePath.wc_str());
			if (!attributes.Equals(INVALID_FILE_ATTRIBUTES) && attributes.Contains(FILE_ATTRIBUTE_DIRECTORY))
			{
				if (flags.Contains(FSActionFlag::Recursive))
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
					}, {}, FSActionFlag::Recursive);
					directories.emplace_back(path);

					size_t failCount = 0;
					while (!shouldExit && !directories.empty())
					{
						for (size_t i = directories.size() - 1; i != 0; i--)
						{
							if (DoRemoveDirectory(directories[i]))
							{
								directories.erase(directories.begin() + i);
							}
							else
							{
								failCount++;
								if (*Win32Error::GetLastError() != ERROR_DIR_NOT_EMPTY)
								{
									shouldExit = true;
									break;
								}
							}
						}

						// Not be the best solution but anyway
						if (failCount >= 10000)
						{
							return false;
						}
					}
					return !shouldExit && directories.empty();
				}
				else
				{
					return DoRemoveDirectory(sourcePath);
				}
			}
			return false;
		});
	}

	std::unique_ptr<IStream> NativeFileSystem::GetStream(const FSPath& path,
														 FlagSet<IOStreamAccess> access,
														 IOStreamDisposition disposition,
														 FlagSet<IOStreamShare> share,
														 FlagSet<IOStreamFlag> streamFlags,
														 FlagSet<FSActionFlag> flags)
	{
		if (IsNull())
		{
			return nullptr;
		}

		return DoWithResolvedPath1(m_CurrentDirectory, path, [&](const FSPath& path) -> std::unique_ptr<IStream>
		{
			auto fileStream = std::make_unique<NativeFileStream>(path, access, disposition, share, streamFlags);
			if (!*fileStream && flags.Contains(FSActionFlag::CreateDirectoryTree) && fileStream->GetLastNativeError().IsSameAs<Win32Error>(ERROR_PATH_NOT_FOUND))
			{
				if (streamFlags.Contains(IOStreamFlag::AllowDirectories))
				{
					CreateDirectory(path);
				}
				else
				{
					CreateDirectory(path.GetParent());
				}
				fileStream = std::make_unique<NativeFileStream>(path, access, disposition, share, streamFlags);
			}

			if (*fileStream)
			{
				if (auto stream = std::unique_ptr<IStream>(fileStream->QueryInterface<IStream>().get()))
				{
					fileStream.release();
					return stream;
				}
			}
			return nullptr;
		});
	}

	// IFileIDSystem
	bool NativeFileSystem::ItemExist(const UniversallyUniqueID& id) const
	{
		if (IsNull())
		{
			return false;
		}

		NativeFileStream file;
		return OpenFileByID(m_CurrentVolume, id, file);
	}
	bool NativeFileSystem::FileExist(const UniversallyUniqueID& id) const
	{
		if (IsNull())
		{
			return false;
		}

		NativeFileStream file;
		if (OpenFileByID(m_CurrentVolume, id, file))
		{
			return !file.GetAttributes().Contains(FileAttribute::Directory);
		}
		return false;
	}
	bool NativeFileSystem::DirectoryExist(const UniversallyUniqueID& id) const
	{
		if (IsNull())
		{
			return false;
		}

		NativeFileStream file;
		if (OpenFileByID(m_CurrentVolume, id, file))
		{
			return file.GetAttributes().Contains(FileAttribute::Directory);
		}
		return false;
	}
	
	FileItem NativeFileSystem::GetItem(const UniversallyUniqueID& id) const
	{
		if (IsNull())
		{
			return {};
		}

		NativeFileStream file;
		if (OpenFileByID(m_CurrentVolume, id, file))
		{
			return FileSystem::Private::ConvertFileInfo(file.GetHandle(), id);
		}
		return {};
	}

	std::unique_ptr<IStream> NativeFileSystem::GetStream(const UniversallyUniqueID& id,
														 FlagSet<IOStreamAccess> access,
														 IOStreamDisposition disposition,
														 FlagSet<IOStreamShare> share,
														 FlagSet<IOStreamFlag> streamFlags,
														 FlagSet<FSActionFlag> flags)
	{
		if (IsNull())
		{
			return nullptr;
		}

		if (id)
		{
			auto fileStream = std::make_unique<NativeFileStream>();
			if (OpenFileByID(m_CurrentVolume, id, *fileStream, access, disposition, share, streamFlags))
			{
				if (auto stream = std::unique_ptr<IStream>(fileStream->QueryInterface<IStream>().get()))
				{
					fileStream.release();
					return stream;
				}
			}
		}
		return nullptr;
	}

	// NativeFileSystem
	bool NativeFileSystem::IsInUse(const FSPath& path) const
	{
		if (IsNull())
		{
			return false;
		}

		return path.IsAbsolute() && NativeFileStream(path, IOStreamAccess::Read, IOStreamDisposition::OpenExisting, IOStreamShare::None);
	}
	size_t NativeFileSystem::EnumStreams(const FSPath& path, TEnumStreamsFunc func) const
	{
		if (IsNull())
		{
			return 0;
		}

		if (path.IsAbsolute())
		{
			size_t counter = 0;
			String pathString = path.GetFullPathWithNS(FSPathNamespace::Win32File);

			WIN32_FIND_STREAM_DATA streamInfo = {};
			HANDLE handle = ::FindFirstStreamW(pathString.wc_str(), STREAM_INFO_LEVELS::FindStreamInfoStandard, &streamInfo, 0);
			if (handle && handle != INVALID_HANDLE_VALUE)
			{
				Utility::ScopeGuard atExit([&]()
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

	bool NativeFileSystem::CopyDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func, FlagSet<FSActionFlag> flags) const
	{
		if (IsNull())
		{
			return false;
		}

		if (source.IsAbsolute() && destination.IsAbsolute())
		{
			return FileSystem::Private::CopyOrMoveDirectoryTree(const_cast<NativeFileSystem&>(*this), source, destination, std::move(func), flags, false);
		}
		return false;
	}
	bool NativeFileSystem::MoveDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func, FlagSet<FSActionFlag> flags)
	{
		if (IsNull())
		{
			return false;
		}

		if (source.IsAbsolute() && destination.IsAbsolute())
		{
			return FileSystem::Private::CopyOrMoveDirectoryTree(*this, source, destination, std::move(func), flags, true);
		}
		return false;
	}
}

namespace kxf
{
	bool ScopedNativeFileSystem::IsNull() const
	{
		return m_CurrentDirectory.IsNull() || m_CurrentVolume.IsNull();
	}
}
