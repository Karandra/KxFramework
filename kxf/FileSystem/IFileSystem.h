#pragma once
#include "Common.h"
#include "FSPath.h"
#include "StorageVolume.h"
#include "kxf/General/BinarySize.h"
#include "kxf/General/LocallyUniqueID.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/RTTI/QueryInterface.h"
#include <wx/datetime.h>

namespace kxf
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

namespace kxf
{
	class KX_API IFileSystem: public RTTI::Interface<IFileSystem>
	{
		KxDeclareIID(IFileSystem, {0x950f74ec, 0x6352, 0x47a1, {0xac, 0x8f, 0x43, 0x6a, 0x6, 0x1e, 0x9f, 0x65}});

		public:
			using TEnumItemsFunc = std::function<bool(FileItem)>;
			using TCopyItemFunc = std::function<bool(BinarySize, BinarySize)>;

		public:
			virtual ~IFileSystem() = default;

		public:
			virtual bool ItemExist(const FSPath& path) const = 0;
			virtual bool FileExist(const FSPath& path) const = 0;
			virtual bool DirectoryExist(const FSPath& path) const = 0;

			virtual FileItem GetItem(const FSPath& path) const = 0;
			virtual size_t EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPathQuery& query = {}, FlagSet<FSEnumItemsFlag> flags = {}) const = 0;
			virtual bool IsDirectoryEmpty(const FSPath& directory) const = 0;
			
			virtual bool CreateDirectory(const FSPath& path) = 0;
			virtual bool ChangeAttributes(const FSPath& path, FlagSet<FileAttribute> attributes) = 0;
			virtual bool ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) = 0;

			virtual bool CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) = 0;
			virtual bool MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) = 0;
			virtual bool RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSCopyItemFlag> flags = {}) = 0;
			virtual bool RemoveItem(const FSPath& path) = 0;

			virtual std::unique_ptr<wxInputStream> OpenToRead(const FSPath& path) const = 0;
			virtual std::unique_ptr<wxOutputStream> OpenToWrite(const FSPath& path) = 0;
	};

	class KX_API IFileIDSystem: public RTTI::Interface<IFileSystem>
	{
		KxDeclareIID(IFileIDSystem, {0x8a4f7e63, 0x6092, 0x4859, {0xa1, 0x74, 0x25, 0x8, 0x7a, 0x4a, 0x90, 0xcb}});

		public:
			virtual ~IFileIDSystem() = default;

		public:
			virtual UniversallyUniqueID GetLookupScope() const = 0;

			virtual bool ItemExist(const UniversallyUniqueID& id) const = 0;
			virtual bool FileExist(const UniversallyUniqueID& id) const = 0;
			virtual bool DirectoryExist(const UniversallyUniqueID& id) const = 0;

			virtual FileItem GetItem(const UniversallyUniqueID& id) const = 0;
			virtual size_t EnumItems(const UniversallyUniqueID& id, IFileSystem::TEnumItemsFunc func, FlagSet<FSEnumItemsFlag> flags = {}) const = 0;
			virtual bool IsDirectoryEmpty(const UniversallyUniqueID& id) const = 0;

			virtual bool ChangeAttributes(const UniversallyUniqueID& id, FlagSet<FileAttribute> attributes) = 0;
			virtual bool ChangeTimestamp(const UniversallyUniqueID& id, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) = 0;

			virtual bool CopyItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, IFileSystem::TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) = 0;
			virtual bool MoveItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, IFileSystem::TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) = 0;
			virtual bool RemoveItem(const UniversallyUniqueID& id) = 0;

			virtual std::unique_ptr<wxInputStream> OpenToRead(const UniversallyUniqueID& id) const = 0;
			virtual std::unique_ptr<wxOutputStream> OpenToWrite(const UniversallyUniqueID& id) = 0;
	};
}
