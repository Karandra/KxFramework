#pragma once
#include "Common.h"
#include "IFileSystem.h"
#include "FileItem.h"

namespace KxFramework
{
	class NativeFileSystem: public IFileSystem
	{
		public:
			NativeFileSystem& GetInstance()
			{
				static NativeFileSystem instance;
				return instance;
			}

		public:
			// IFileSystem
			FileItem GetItem(const FSPath& path) const override;
			size_t EnumItems(const FSPath& directory, std::function<bool(const FileItem&)> func, const wxString& query = {}, FSEnumItemsFlag flags = FSEnumItemsFlag::None) const override;
			
			bool ChangeAttributes(const FSPath& path, FileAttribute attributes) override;
			bool ChangeTimestamp(const FSPath& path, const wxDateTime& creationTime, const wxDateTime& modificationTime, const wxDateTime& lastAccessTime) override;

		public:
			bool IsInUse(const FSPath& path) const;
	};
}
