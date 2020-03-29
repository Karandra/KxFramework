#pragma once
#include "Common.h"
#include "IFileSystem.h"
#include "FileItem.h"

namespace KxFramework
{
	class KX_API StatefulFileSystem
	{
		public:
			IFileSystem& m_FileSystem;
			FSPath m_CurrentLocation;

		public:
			StatefulFileSystem(IFileSystem& fileSystem, const FSPath& path = {})
				:m_FileSystem(fileSystem), m_CurrentLocation(path)
			{
			}
			virtual ~StatefulFileSystem() = default;

		public:
			const IFileSystem& GetFileSystem() const
			{
				return m_FileSystem;
			}
			IFileSystem& GetFileSystem()
			{
				return m_FileSystem;
			}

			bool Navigate(const FSPath& path);
			bool NavigateUp();
			FSPath GetCurrentLocation() const;

		public:
			// IFileSystem
			FileItem GetItem(const FSPath& path) const
			{
				return m_FileSystem.GetItem(path);
			}
			size_t EnumItems(std::function<bool(const FileItem&)> func, const wxString& query = {}, FSEnumItemsFlag flags = FSEnumItemsFlag::None) const
			{
				return m_FileSystem.EnumItems(m_CurrentLocation, std::move(func), query, flags);
			}
			
			bool CreateDirectory()
			{
				return m_FileSystem.CreateDirectory(m_CurrentLocation);
			}
			bool ChangeAttributes(FileAttribute attributes)
			{
				return m_FileSystem.ChangeAttributes(m_CurrentLocation, attributes);
			}
			bool SetFileTime(const wxDateTime& creationTime, const wxDateTime& modificationTime, const wxDateTime& lastAccessTime)
			{
				return m_FileSystem.ChangeTimestamp(m_CurrentLocation, creationTime, modificationTime, lastAccessTime);
			}
	};

	class KX_API OwningStatefulFileSystem: public StatefulFileSystem
	{
		private:
			std::unique_ptr<IFileSystem> m_FileSystem;

		public:
			OwningStatefulFileSystem(std::unique_ptr<IFileSystem> fileSystem, const FSPath& path = {})
				:StatefulFileSystem(*fileSystem, path), m_FileSystem(std::move(fileSystem))
			{
			}
	};
}
