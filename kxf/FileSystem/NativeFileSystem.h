#pragma once
#include "Common.h"
#include "IFileSystem.h"
#include "FileItem.h"
#include "StorageVolume.h"

namespace kxf
{
	class NativeFileSystem: public RTTI::ImplementInterface<NativeFileSystem, IFileSystem, IFileIDSystem>
	{
		public:
			using TCopyDirectoryTreeFunc = std::function<bool(FSPath, FSPath, BinarySize, BinarySize)>;
			using TEnumStreamsFunc = std::function<bool(String, BinarySize)>;

		public:
			static FSPath GetExecutableDirectory();
			static FSPath GetWorkingDirectory();
			static bool SetWorkingDirectory(const FSPath& directory);

		private:
			StorageVolume m_CurrentVolume;
			FSPath m_CurrentDirectory;

		public:
			NativeFileSystem(StorageVolume volume = {}) noexcept
				:m_CurrentVolume(std::move(volume))
			{
				m_CurrentDirectory.SetVolume(m_CurrentVolume);
			}
			NativeFileSystem(const UniversallyUniqueID& scope) noexcept
				:m_CurrentVolume(scope)
			{
				m_CurrentDirectory.SetVolume(m_CurrentVolume);
			}
			NativeFileSystem(const FSPath& currentDirectory) noexcept
				:m_CurrentVolume(currentDirectory.GetAsVolume()), m_CurrentDirectory(currentDirectory)
			{
			}

		public:
			// IFileSystem
			FSPath GetCurrentDirectory() const override
			{
				return m_CurrentDirectory;
			}
			FSPath ResolvePath(const FSPath& relativePath) const override;

			bool ItemExist(const FSPath& path) const override;
			bool FileExist(const FSPath& path) const override;
			bool DirectoryExist(const FSPath& path) const override;

			FileItem GetItem(const FSPath& path) const override;
			size_t EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPathQuery& query = {}, FlagSet<FSActionFlag> flags = {}) const override;
			bool IsDirectoryEmpty(const FSPath& directory) const override;

			bool CreateDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) override;
			bool ChangeAttributes(const FSPath& path, FlagSet<FileAttribute> attributes) override;
			bool ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) override;

			bool CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSActionFlag> flags = {}) override;
			bool MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSActionFlag> flags = {}) override;
			bool RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSActionFlag> flags = {}) override;
			bool RemoveItem(const FSPath& path) override;
			bool RemoveDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) override;

			std::unique_ptr<IStream> GetStream(const FSPath& path,
											   FlagSet<IOStreamAccess> access,
											   IOStreamDisposition disposition,
											   FlagSet<IOStreamShare> share = IOStreamShare::Read,
											   FlagSet<IOStreamFlag> streamFlags = IOStreamFlag::None,
											   FlagSet<FSActionFlag> flags = {}
			) override;
			using IFileSystem::OpenToRead;
			using IFileSystem::OpenToWrite;

		public:
			// IFileIDSystem
			UniversallyUniqueID GetLookupScope() const override
			{
				return m_CurrentVolume.GetUniqueID();
			}

			bool ItemExist(const UniversallyUniqueID& id) const override;
			bool FileExist(const UniversallyUniqueID& id) const override;
			bool DirectoryExist(const UniversallyUniqueID& id) const override;

			FileItem GetItem(const UniversallyUniqueID& id) const override;
			size_t EnumItems(const UniversallyUniqueID& id, TEnumItemsFunc func, FlagSet<FSActionFlag> flags = {}) const override
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

			bool CopyItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, TCopyItemFunc func = {}, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool MoveItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, TCopyItemFunc func = {}, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool RemoveItem(const UniversallyUniqueID& id) override
			{
				return false;
			}
			bool RemoveDirectory(const UniversallyUniqueID& id, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}

			std::unique_ptr<IStream> GetStream(const UniversallyUniqueID& id,
											   FlagSet<IOStreamAccess> access,
											   IOStreamDisposition disposition,
											   FlagSet<IOStreamShare> share = IOStreamShare::Read,
											   FlagSet<IOStreamFlag> streamFlags = IOStreamFlag::None,
											   FlagSet<FSActionFlag> flags = {}
			) override;
			using IFileIDSystem::OpenToRead;
			using IFileIDSystem::OpenToWrite;

		public:
			// NativeFileSystem
			StorageVolume GetCurrentVolume() const noexcept
			{
				return m_CurrentVolume;
			}

			bool IsInUse(const FSPath& path) const;
			size_t EnumStreams(const FSPath& path, TEnumStreamsFunc func) const;

			bool CopyDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func = {}, FlagSet<FSActionFlag> flags = {}) const;
			bool MoveDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func = {}, FlagSet<FSActionFlag> flags = {});
	};
}
