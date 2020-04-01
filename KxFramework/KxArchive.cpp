#include "KxStdAfx.h"
#include "KxFramework/KxArchive.h"
#include <KxFramework/KxFileStream.h>
#include <KxFramework/KxStreamDelegate.h>
#include "Kx/FileSystem/NativeFileSystemUtility.h"

namespace KxArchive
{
	class ExtractorCallbackBase: public KxFramework::RTTI::ImplementInterface<ExtractorCallbackBase, IExtractionCallback>
	{
		protected:
			IArchiveItems& m_ArchiveItems;

		public:
			ExtractorCallbackBase(IArchiveExtraction& archive)
				:m_ArchiveItems(*archive.QueryInterface<IArchiveItems>())
			{
			}
	};

	class FileExtractionCallback: public ExtractorCallbackBase
	{
		private:
			FSPath m_Directory;
			FileItem m_FileItem;
			KxFileStream m_Stream;

		public:
			FileExtractionCallback(IArchiveExtraction& archive, const FSPath& directory = {})
				:ExtractorCallbackBase(archive), m_Directory(directory)
			{
			}

		public:
			KxDelegateOutputStream OnGetStream(FileIndex fileIndex) override
			{
				m_FileItem = m_ArchiveItems.GetItem(fileIndex);
				if (m_FileItem)
				{
					// Get target path
					FSPath targetPath = GetTargetPath(m_FileItem);
					if (m_FileItem.IsDirectory())
					{
						// Creating a directory here supports having empty directories
						NativeFileSystem::Get().CreateDirectory(targetPath);
						return nullptr;
					}
					else
					{
						NativeFileSystem::Get().CreateDirectory(targetPath.GetParent());
						m_Stream.Open(targetPath, KxFileStream::Access::Write, KxFileStream::Disposition::CreateAlways, KxFileStream::Share::Read);
						return m_Stream;
					}
				}
				return nullptr;
			}
			bool OnOperationCompleted(FileIndex fileIndex, wxOutputStream& stream) override
			{
				if (m_FileItem)
				{
					m_Stream.SetAttributes(FileSystem::NativeUtility::MapFileAttributes(m_FileItem.GetAttributes()));
					m_Stream.SetFileTime(m_FileItem.GetCreationTime(), m_FileItem.GetModificationTime(), m_FileItem.GetLastAccessTime());
					m_Stream.Close();

					return true;
				}
				return false;
			}
			
			virtual FSPath GetTargetPath(const FileItem& fileItem) const
			{
				return m_Directory / fileItem.GetFullPath();
			}
	};
	class SingleFileExtractionCallback: public FileExtractionCallback
	{
		private:
			FSPath m_TargetPath;

		public:
			SingleFileExtractionCallback(IArchiveExtraction& archive, const FSPath& targetPath)
				:FileExtractionCallback(archive), m_TargetPath(targetPath)
			{
			}

		public:
			FSPath GetTargetPath(const FileItem& fileItem) const override
			{
				return m_TargetPath;
			}
	};
	class SingleStreamExtractionCallback: public ExtractorCallbackBase
	{
		private:
			wxOutputStream& m_Stream;

		public:
			SingleStreamExtractionCallback(IArchiveExtraction& archive, wxOutputStream& stream)
				:ExtractorCallbackBase(archive), m_Stream(stream)
			{
			}

		public:
			KxDelegateOutputStream OnGetStream(FileIndex fileIndex) override
			{
				FileItem fileItem = m_ArchiveItems.GetItem(fileIndex);
				if (fileItem && !fileItem.IsDirectory())
				{
					return m_Stream;
				}
				return nullptr;
			}
			bool OnOperationCompleted(FileIndex fileIndex, wxOutputStream& stream) override
			{
				return true;
			}
	};
}

namespace KxArchive
{
	FileItem IArchiveItems::FindItem(const FSPathQuery& query) const
	{
		FileItem result;
		EnumItems({}, [&](FileItem item)
		{
			result = std::move(item);
			return false;
		}, query, FSEnumItemsFlag::Recursive);
		return result;
	}
	FileItem IArchiveItems::FindItem(const FSPath& directory, const FSPathQuery& query) const
	{
		FileItem result;
		EnumItems(directory, [&](FileItem item)
		{
			result = std::move(item);
			return false;
		}, query, FSEnumItemsFlag::Recursive);
		return result;
	}
}

namespace KxArchive
{
	bool IArchiveExtraction::ExtractToDirectory(const FSPath& directory) const
	{
		FileExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), directory);
		return Extract(callback);
	}
	bool IArchiveExtraction::ExtractToDirectory(const FSPath& directory, FileIndexView files) const
	{
		FileExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), directory);
		return Extract(callback, files);
	}

	bool IArchiveExtraction::ExtractToStream(FileIndex fileIndex, wxOutputStream& stream) const
	{
		SingleStreamExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), stream);
		return Extract(callback, fileIndex);
	}
	bool IArchiveExtraction::ExtractToFile(FileIndex fileIndex, const FSPath& targetPath) const
	{
		SingleFileExtractionCallback callback(const_cast<IArchiveExtraction&>(*this), targetPath);
		return Extract(callback, fileIndex);
	}
}
