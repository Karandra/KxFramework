#pragma once
#include "Common.h"
#include "FSPath.h"
#include "Kx/General/BinarySize.h"
#include "Kx/RTTI/QueryInterface.h"
#include <wx/datetime.h>

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
	Kx_DeclareFlagSet(FSEnumItemsFlag);

	enum class FSCopyItemFlag
	{
		None = 0,

		ReplaceIfExist = 1 << 0,
		NoBuffering = 1 << 1,
	};
	Kx_DeclareFlagSet(FSCopyItemFlag);
}

namespace KxFramework
{
	class KX_API IFileSystem: public RTTI::Interface<IFileSystem>
	{
		KxDecalreIID(IFileSystem, {0x950f74ec, 0x6352, 0x47a1, {0xac, 0x8f, 0x43, 0x6a, 0x6, 0x1e, 0x9f, 0x65}});

		public:
			using TEnumItemsFunc = std::function<bool(FileItem)>;
			using TCopyItemFunc = std::function<bool(BinarySize, BinarySize)>;

		public:
			virtual ~IFileSystem() = default;

		public:
			virtual FileItem GetItem(const FSPath& path) const = 0;
			virtual size_t EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPathQuery& query = {}, FlagSet<FSEnumItemsFlag> flags = {}) const = 0;
			
			virtual bool CreateDirectory(const FSPath& path) = 0;
			virtual bool ChangeAttributes(const FSPath& path, FileAttribute attributes) = 0;
			virtual bool ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) = 0;

			virtual bool CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) = 0;
			virtual bool MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) = 0;
			virtual bool RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSCopyItemFlag> flags = {}) = 0;
			virtual bool RemoveItem(const FSPath& path) = 0;
	};
}
