#pragma once
#include "Common.h"
#include "FSPath.h"
#include "Kx/General/BinarySize.h"

namespace KxFramework
{
	class FileItem;

	enum class FSEnumItemsFlag
	{
		None = 0,

		CaseSensitive = 1 << 0,
		Recursive = 1 << 1,
		LimitToFiles = 1 << 2,
		LimitToDirectories = 1 << 3,
	};
	enum class FSCopyItemFlag
	{
		None = 0,

		ReplaceIfExist = 1 << 0,
		NoBuffering = 1 << 1,
	};
}

namespace KxEnumClassOperations
{
	KxImplementEnum(KxFramework::FSEnumItemsFlag);
	KxImplementEnum(KxFramework::FSCopyItemFlag);
}

namespace KxFramework
{
	class KX_API IFileSystem
	{
		public:
			using TEnumItemsFunc = std::function<bool(FileItem)>;
			using TCopyItemFunc = std::function<bool(BinarySize, BinarySize)>;

		public:
			virtual ~IFileSystem() = default;

		public:
			virtual FileItem GetItem(const FSPath& path) const = 0;
			virtual size_t EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPathQuery& query = {}, FSEnumItemsFlag flags = FSEnumItemsFlag::None) const = 0;
			
			virtual bool CreateDirectory(const FSPath& path) = 0;
			virtual bool ChangeAttributes(const FSPath& path, FileAttribute attributes) = 0;
			virtual bool ChangeTimestamp(const FSPath& path, const wxDateTime& creationTime, const wxDateTime& modificationTime, const wxDateTime& lastAccessTime) = 0;

			virtual bool CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FSCopyItemFlag flags = FSCopyItemFlag::None) = 0;
			virtual bool MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FSCopyItemFlag flags = FSCopyItemFlag::None) = 0;
			virtual bool RenameItem(const FSPath& source, const FSPath& destination, FSCopyItemFlag flags = FSCopyItemFlag::None) = 0;
			virtual bool RemoveItem(const FSPath& path) = 0;
	};
}
