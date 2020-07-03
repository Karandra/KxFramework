#include "stdafx.h"
#include "NativeFSUtility.h"
#include "kxf/IO/FileStream.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/Utility/CallAtScopeExit.h"

namespace
{
	using namespace kxf;

	void GetFileID(FileItem& fileItem, HANDLE fileHandle, const BY_HANDLE_FILE_INFORMATION& fileInfo)
	{
		// "If you want to use GUIDs to identify your files, then nobody's stopping you": https://devblogs.microsoft.com/oldnewthing/20110228-00/?p=11363
		// Raymond Chen shows us use of NTFS ObjectIDs but I'm not sure it's the best idea to always use them
		// https://stackoverflow.com/questions/62440438/getfileinformationbyhandleex-fileidinfo-vs-deviceiocontrol-fsctl-create-or-get-o

		fileItem.SetUniqueID(LocallyUniqueID(Utility::IntFromLowHigh<uint64_t>(fileInfo.nFileIndexLow, fileInfo.nFileIndexHigh)));
		if (System::IsWindows8OrGreater())
		{
			FILE_ID_INFO fileIDInfo = {};
			if (::GetFileInformationByHandleEx(fileHandle, FILE_INFO_BY_HANDLE_CLASS::FileIdInfo, &fileIDInfo, sizeof(fileIDInfo)))
			{
				fileItem.SetUniqueID(UniversallyUniqueID::CreateFromInt128(fileIDInfo.FileId.Identifier));
			}
		}
	}
}

namespace kxf::FileSystem::Private
{
	uint32_t GetFileAttributes(const FSPath& path)
	{
		String pathName = path.GetFullPathWithNS(FSPathNamespace::Win32File);
		return ::GetFileAttributesW(pathName.wc_str());
	}

	bool IsValidFindItem(const WIN32_FIND_DATAW& findInfo) noexcept
	{
		std::wstring_view name = findInfo.cFileName;
		return !(findInfo.dwFileAttributes == INVALID_FILE_ATTRIBUTES || name.empty() || name == L".." || name == L".");
	}
	HANDLE CallFindFirstFile(const String& query, WIN32_FIND_DATAW& findInfo, bool isCaseSensitive)
	{
		const DWORD searchFlags = FIND_FIRST_EX_LARGE_FETCH|(isCaseSensitive ? FIND_FIRST_EX_CASE_SENSITIVE : 0);
		return ::FindFirstFileExW(query.wc_str(), FindExInfoBasic, &findInfo, FINDEX_SEARCH_OPS::FindExSearchNameMatch, nullptr, searchFlags);
	}

	FileItem ConvertFileInfo(const WIN32_FIND_DATAW& findInfo, const FSPath& location, UniversallyUniqueID id, bool forceFetchID)
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

		// Unique ID
		if (id)
		{
			fileItem.SetUniqueID(std::move(id));
		}
		else if (forceFetchID)
		{
			// Switch to a different directory enumeration method to avoid opening the file here to get its ID
			BY_HANDLE_FILE_INFORMATION fileInfo = {};
			FileStream stream(path, FileStreamAccess::ReadAttributes, FileStreamDisposition::OpenExisting, FileStreamShare::Everything, FileStreamFlags::BackupSemantics);
			if (stream && ::GetFileInformationByHandle(stream.GetHandle(), &fileInfo))
			{
				GetFileID(fileItem, stream.GetHandle(), fileInfo);
			}
		}

		// Assign path
		fileItem.SetFullPath(std::move(path));

		return fileItem;
	}
	FileItem ConvertFileInfo(HANDLE fileHandle, UniversallyUniqueID id)
	{
		FileStream stream;
		if (stream.AttachHandle(fileHandle))
		{
			Utility::CallAtScopeExit atExit= [&]()
			{
				stream.DetachHandle();
			};

			// File item and path
			FileItem fileItem(stream.GetFileSystemPath());

			BY_HANDLE_FILE_INFORMATION fileInfo = {};
			if (::GetFileInformationByHandle(fileHandle, &fileInfo))
			{
				// Attributes and reparse point
				const bool isDirectory = fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
				fileItem.SetAttributes(MapFileAttributes(fileInfo.dwFileAttributes));
				if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
				{
					FILE_ATTRIBUTE_TAG_INFO attributeTagInfo = {};
					if (::GetFileInformationByHandleEx(fileHandle, FILE_INFO_BY_HANDLE_CLASS::FileAttributeTagInfo, &attributeTagInfo, sizeof(attributeTagInfo)))
					{
						fileItem.SetReparsePointTags(MapReparsePointTags(attributeTagInfo.ReparseTag));
					}
				}

				// File size
				if (!isDirectory)
				{
					fileItem.SetSize(BinarySize::FromBytes(Utility::IntFromLowHigh<uint64_t>(fileInfo.nFileSizeLow, fileInfo.nFileSizeHigh)));
				}

				// Date and time
				fileItem.SetCreationTime(ConvertDateTime(fileInfo.ftCreationTime));
				fileItem.SetModificationTime(ConvertDateTime(fileInfo.ftLastWriteTime));
				fileItem.SetLastAccessTime(ConvertDateTime(fileInfo.ftLastAccessTime));

				// Compressed file size
				if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
				{
					FILE_COMPRESSION_INFO compressionInfo = {};
					if (::GetFileInformationByHandleEx(fileHandle, FILE_INFO_BY_HANDLE_CLASS::FileCompressionInfo, &compressionInfo, sizeof(compressionInfo)))
					{
						fileItem.SetCompressedSize(BinarySize::FromBytes(compressionInfo.CompressedFileSize.QuadPart));
					}
				}

				// Unique ID
				if (id)
				{
					fileItem.SetUniqueID(std::move(id));
				}
				else
				{
					GetFileID(fileItem, fileHandle, fileInfo);
				}
			}
			return fileItem;
		}
		return {};
	}

	DWORD CALLBACK CopyCallback(LARGE_INTEGER TotalFileSize,
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
	bool CopyOrMoveDirectoryTree(NativeFileSystem& fileSystem,
								 const FSPath& source,
								 const FSPath& destination,
								 NativeFileSystem::TCopyDirectoryTreeFunc func,
								 FlagSet<FSActionFlag> flags,
								 bool move)
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
		}, {}, FSActionFlag::Recursive) != 0;
	}
}
