#pragma once
#include "Common.h"
#include "IFileSystem.h"
#include "FileItem.h"

namespace kxf
{
	class NativeFileSystem: public RTTI::ImplementInterface<NativeFileSystem, IFileSystem, IFileIDSystem>
	{
		public:
			using TCopyDirectoryTreeFunc = std::function<bool(FSPath, FSPath, BinarySize, BinarySize)>;
			using TEnumStreamsFunc = std::function<bool(String, BinarySize)>;

		private:
			UniversallyUniqueID m_LookupScope;

		public:
			NativeFileSystem(const UniversallyUniqueID& scope  = {}) noexcept
				:m_LookupScope(scope)
			{
			}

		public:
			// IFileSystem
			bool ItemExist(const FSPath& path) const;
			bool FileExist(const FSPath& path) const;
			bool DirectoryExist(const FSPath& path) const;

			FileItem GetItem(const FSPath& path) const override;
			size_t EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPathQuery& query = {}, FlagSet<FSEnumItemsFlag> flags = {}) const override;
			bool IsDirectoryEmpty(const FSPath& directory) const override;

			bool CreateDirectory(const FSPath& path) override;
			bool ChangeAttributes(const FSPath& path, FlagSet<FileAttribute> attributes) override;
			bool ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) override;

			bool CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) override;
			bool MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) override;
			bool RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSCopyItemFlag> flags = {}) override;
			bool RemoveItem(const FSPath& path) override;

			std::unique_ptr<wxInputStream> OpenToRead(const FSPath& path) override;
			std::unique_ptr<wxOutputStream> OpenToWrite(const FSPath& path) override;

		public:
			// IFileIDSystem
			UniversallyUniqueID GetLookupScope() const override
			{
				return m_LookupScope;
			}

			bool ItemExist(const UniversallyUniqueID& id) const override;
			bool FileExist(const UniversallyUniqueID& id) const override;
			bool DirectoryExist(const UniversallyUniqueID& id) const override;

			FileItem GetItem(const UniversallyUniqueID& id) const override;
			size_t EnumItems(const UniversallyUniqueID& id, TEnumItemsFunc func, FlagSet<FSEnumItemsFlag> flags = {}) const override
			{
				return 0;
			}
			bool IsDirectoryEmpty(const UniversallyUniqueID& id) const override
			{
				return false;
			}

			bool ChangeAttributes(const UniversallyUniqueID& id, FlagSet<FileAttribute> attributes) override
			{
				return false;
			}
			bool ChangeTimestamp(const UniversallyUniqueID& id, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime)  override
			{
				return false;
			}

			bool CopyItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) override
			{
				return false;
			}
			bool MoveItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) override
			{
				return false;
			}
			bool RemoveItem(const UniversallyUniqueID& id) override
			{
				return false;
			}

			std::unique_ptr<wxInputStream> OpenToRead(const UniversallyUniqueID& id) override;
			std::unique_ptr<wxOutputStream> OpenToWrite(const UniversallyUniqueID& id) override;

		public:
			bool IsInUse(const FSPath& path) const;
			size_t EnumStreams(const FSPath& path, TEnumStreamsFunc func) const;

			bool RemoveDirectoryTree(const FSPath& path);
			bool CopyDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) const;
			bool MoveDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func = {}, FlagSet<FSCopyItemFlag> flags = {});

			FSPath GetExecutableDirectory() const;
			FSPath GetWorkingDirectory() const;
			bool SetWorkingDirectory(const FSPath& directory) const;
	};
}
