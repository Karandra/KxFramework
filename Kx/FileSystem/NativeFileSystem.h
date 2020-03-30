#pragma once
#include "Common.h"
#include "IFileSystem.h"
#include "FileItem.h"

namespace KxFramework
{
	class NativeFileSystem: public IFileSystem
	{
		public:
			using TCopyDirectoryTreeFunc = std::function<bool(FSPath, FSPath, BinarySize, BinarySize)>;
			using TEnumStreamsFunc = std::function<bool(const wxString&, BinarySize)>;

		public:
			NativeFileSystem& GetInstance()
			{
				static NativeFileSystem instance;
				return instance;
			}

		protected:
			NativeFileSystem() = default;
			~NativeFileSystem() = default;

		public:
			// IFileSystem
			FileItem GetItem(const FSPath& path) const override;
			size_t EnumItems(const FSPath& directory, TEnumItemsFunc func, const wxString& query = {}, FSEnumItemsFlag flags = FSEnumItemsFlag::None) const override;
			
			bool CreateDirectory(const FSPath& path) override;
			bool ChangeAttributes(const FSPath& path, FileAttribute attributes) override;
			bool ChangeTimestamp(const FSPath& path, const wxDateTime& creationTime, const wxDateTime& modificationTime, const wxDateTime& lastAccessTime) override;

			bool CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FSCopyItemFlag flags = FSCopyItemFlag::None) override;
			bool MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FSCopyItemFlag flags = FSCopyItemFlag::None) override;
			bool RenameItem(const FSPath& source, const FSPath& destination, FSCopyItemFlag flags = FSCopyItemFlag::None) override;
			bool RemoveItem(const FSPath& path) override;

		public:
			bool IsInUse(const FSPath& path) const;
			size_t EnumStreams(const FSPath& path, TEnumStreamsFunc func) const;

			bool RemoveDirectoryTree(const FSPath& path);
			bool CopyDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func = {}, FSCopyItemFlag flags = FSCopyItemFlag::None) const;
			bool MoveDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func = {}, FSCopyItemFlag flags = FSCopyItemFlag::None);
	};
}
