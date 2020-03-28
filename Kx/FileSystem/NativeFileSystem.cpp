#include "KxStdAfx.h"
#include "NativeFileSystem.h"
#include "Kx/Utility/Common.h"
#include "Kx/Utility/CallAtScopeExit.h"

namespace KxFramework
{
	HANDLE CallFindFirstFile(const wxString& query, WIN32_FIND_DATAW& findInfo, bool isCaseSensitive = false)
	{
		const DWORD searchFlags = FIND_FIRST_EX_LARGE_FETCH|(isCaseSensitive ? FIND_FIRST_EX_CASE_SENSITIVE : 0);
		return ::FindFirstFileExW(query.wc_str(), FindExInfoBasic, &findInfo, FINDEX_SEARCH_OPS::FindExSearchNameMatch, nullptr, searchFlags);
	}
	bool CallFindNextFile(HANDLE handle, WIN32_FIND_DATAW& findInfo)
	{
		return ::FindNextFileW(handle, &findInfo);
	}
	bool CallFindClose(HANDLE handle)
	{
		return ::FindClose(handle);
	}

	uint32_t GetFileAttributes(const FSPath& filePath)
	{
		wxString path = filePath.GetFullPathWithNS(FSPathNamespace::Win32File);
		return ::GetFileAttributesW(path.wc_str());
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
}

namespace KxFramework
{
	bool NativeFileSystem::DoesExist(const FSPath& path) const
	{
		return GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES;
	}
	bool NativeFileSystem::DoesFileExist(const FSPath& path) const
	{
		const uint32_t attributes = GetFileAttributes(path);
		return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
	}
	bool NativeFileSystem::DoesDirectoryExist(const FSPath& path) const
	{
		const uint32_t attributes = GetFileAttributes(path);
		return attributes != INVALID_FILE_ATTRIBUTES && attributes & FILE_ATTRIBUTE_DIRECTORY;
	}

	FileItem NativeFileSystem::GetItem(const FSPath& path) const
	{
		WIN32_FIND_DATAW findInfo = {};
		HANDLE handle = CallFindFirstFile(path.GetFullPathWithNS(), findInfo);
		if (handle && handle != INVALID_HANDLE_VALUE)
		{
			Utility::CallAtScopeExit atExit([&]()
			{
				CallFindClose(handle);
			});

			if (IsValidFindItem(findInfo))
			{
				return ConvertFileInfo(findInfo, path);
			}
		}
		return {};
	}
	size_t NativeFileSystem::EnumItems(const FSPath& directory, std::function<bool(const FileItem&)> func, const wxString& query, FSEnumItemsFlag flags) const
	{
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
					CallFindClose(handle);
				});

				do
				{
					// Skip invalid items and current and parent directory links
					if (IsValidFindItem(findInfo))
					{
						if (func(ConvertFileInfo(findInfo, directory)))
						{
							counter++;
						}
						else
						{
							return;
						}

						if (findInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && flags & FSEnumItemsFlag::Recursive)
						{
							childDirectories.emplace_back(findInfo.cFileName).EnsureNamespaceSet(directory.GetNamespace());
						}
					}
				}
				while (CallFindNextFile(handle, findInfo));
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
}
